
#include "message_loop.h"

#include "base/lazy_instance.h"
#include "base/metric/histogram.h"
#include "base/threading/thread_local.h"

#include "message_pump_default.h"

namespace
{

    // ������(lazily created)��TLS, ���ڿ��ٷ����̵߳���Ϣѭ��(�������).
    // �൱��һ����ȫ������ľ�̬���캯��.
    base::LazyInstance<base::ThreadLocalPointer<MessageLoop> > lazy_tls_ptr(
        base::LINKER_INITIALIZED);

    // Logical events for Histogram profiling. Run with -message-loop-histogrammer
    // to get an accounting of messages and actions taken on each thread.
    const int kTaskRunEvent = 0x1;
    const int kTimerEvent = 0x2;

    // Provide range of message IDs for use in histogramming and debug display.
    const int kLeastNonZeroMessageId = 1;
    const int kMaxMessageId = 1099;
    const int kNumberOfDistinctMessagesDisplayed = 1100;

    // Provide a macro that takes an expression (such as a constant, or macro
    // constant) and creates a pair to initalize an array of pairs.  In this case,
    // our pair consists of the expressions value, and the "stringized" version
    // of the expression (i.e., the exrpression put in quotes).  For example, if
    // we have:
    //    #define FOO 2
    //    #define BAR 5
    // then the following:
    //    VALUE_TO_NUMBER_AND_NAME(FOO + BAR)
    // will expand to:
    //   {7, "FOO + BAR"}
    // We use the resulting array as an argument to our histogram, which reads the
    // number as a bucket identifier, and proceeds to use the corresponding name
    // in the pair (i.e., the quoted string) when printing out a histogram.
#define VALUE_TO_NUMBER_AND_NAME(name) { name, #name },

    const base::LinearHistogram::DescriptionPair event_descriptions_[] =
    {
        // Provide some pretty print capability in our histogram for our internal
        // messages.

        // A few events we handle (kindred to messages), and used to profile actions.
        VALUE_TO_NUMBER_AND_NAME(kTaskRunEvent)
        VALUE_TO_NUMBER_AND_NAME(kTimerEvent)

        { -1, NULL } // The list must be null terminated, per API to histogram.
    };

    bool enable_histogrammer_ = false;

}

// ���߳�����SEH�쳣ʱ, �ָ��ɵ�δ�����쳣������.
static int SEHFilter(LPTOP_LEVEL_EXCEPTION_FILTER old_filter)
{
    ::SetUnhandledExceptionFilter(old_filter);
    return EXCEPTION_CONTINUE_SEARCH;
}

// ���ص�ǰ��δ�����쳣������ָ��, û�п�ֱ�ӻ�ȡ��API.
static LPTOP_LEVEL_EXCEPTION_FILTER GetTopSEHFilter()
{
    LPTOP_LEVEL_EXCEPTION_FILTER top_filter = NULL;
    top_filter = ::SetUnhandledExceptionFilter(0);
    ::SetUnhandledExceptionFilter(top_filter);
    return top_filter;
}

MessageLoop::TaskObserver::TaskObserver() {}

MessageLoop::TaskObserver::~TaskObserver() {}

MessageLoop::DestructionObserver::~DestructionObserver() {}

MessageLoop::MessageLoop(Type type)
: type_(type),
nestable_tasks_allowed_(true),
exception_restoration_(false),
state_(NULL),
os_modal_loop_(false),
next_sequence_num_(0)
{
    DCHECK(!current()) << "should only have one message loop per thread";
    lazy_tls_ptr.Pointer()->Set(this);

#define MESSAGE_PUMP_UI new base::MessagePumpForUI()
#define MESSAGE_PUMP_IO new base::MessagePumpForIO()

    if(type_ == TYPE_UI)
    {
        pump_ = MESSAGE_PUMP_UI;
    }
    else if(type_ == TYPE_IO)
    {
        pump_ = MESSAGE_PUMP_IO;
    }
    else
    {
        DCHECK_EQ(TYPE_DEFAULT, type_);
        pump_ = new base::MessagePumpDefault();
    }
}

MessageLoop::~MessageLoop()
{
    DCHECK(this == current());

    DCHECK(!state_);

    // ��������δ���������, ����ע��: ɾ��һ��������ܻᵼ������µ�����(����ͨ��
    // DeleteSoon). ���ǶԱ�ɾ����������������Ĵ�������һ���������. һ����˵, ѭ
    // ��ִ��һ���ξͻ��˳�. �������ʱ����ѭ�����������ֵ, ��������һ������һֱ��
    // ���������, ���Լ����п������µ���ʲô����.
    bool did_work;
    for(int i=0; i<100; ++i)
    {
        DeletePendingTasks();
        ReloadWorkQueue();
        // �������Ϊ��, ����ѭ��.
        did_work = DeletePendingTasks();
        if(!did_work)
        {
            break;
        }
    }
    DCHECK(!did_work);

    // ��DestructionObservers������this�Ļ���.
    FOR_EACH_OBSERVER(DestructionObserver, destruction_observers_,
        WillDestroyCurrentMessageLoop());

    // ����ΪNULL, ���󲻻��ٱ����ʵ�.
    lazy_tls_ptr.Pointer()->Set(NULL);
}

// static
MessageLoop* MessageLoop::current()
{
    return lazy_tls_ptr.Pointer()->Get();
}

void MessageLoop::AddDestructionObserver(DestructionObserver* destruction_observer)
{
    DCHECK(this == current());
    destruction_observers_.AddObserver(destruction_observer);
}

void MessageLoop::RemoveDestructionObserver(
    DestructionObserver* destruction_observer)
{
    DCHECK(this == current());
    destruction_observers_.RemoveObserver(destruction_observer);
}

void MessageLoop::PostTask(Task* task)
{
    PostTask_Helper(task, 0, true);
}

void MessageLoop::PostDelayedTask(Task* task, int64 delay_ms)
{
    PostTask_Helper(task, delay_ms, true);
}

void MessageLoop::PostNonNestableTask(Task* task)
{
    PostTask_Helper(task, 0, false);
}

void MessageLoop::PostNonNestableDelayedTask(Task* task, int64 delay_ms)
{
    PostTask_Helper(task, delay_ms, false);
}

void MessageLoop::Run()
{
    AutoRunState save_state(this);
    RunHandler();
}

void MessageLoop::RunAllPending()
{
    AutoRunState save_state(this);
    state_->quit_received = true; // ����ֱ������ʱ�˳�, ��������ȴ�״̬.
    RunHandler();
}

void MessageLoop::Quit()
{
    DCHECK(current() == this);
    if(state_)
    {
        state_->quit_received = true;
    }
    else
    {
        NOTREACHED() << "Must be inside Run to call Quit";
    }
}

void MessageLoop::QuitNow()
{
    DCHECK(current() == this);
    if(state_)
    {
        pump_->Quit();
    }
    else
    {
        NOTREACHED() << "Must be inside Run to call Quit";
    }
}

void MessageLoop::SetNestableTasksAllowed(bool allowed)
{
    if(nestable_tasks_allowed_ != allowed)
    {
        nestable_tasks_allowed_ = allowed;
        if(!nestable_tasks_allowed_)
        {
            return;
        }
        // ����������Ϣ��.
        pump_->ScheduleWork();
    }
}

bool MessageLoop::NestableTasksAllowed() const
{
    return nestable_tasks_allowed_;
}

bool MessageLoop::IsNested()
{
    return state_->run_depth > 1;
}

void MessageLoop::AddTaskObserver(TaskObserver* task_observer)
{
    DCHECK_EQ(this, current());
    task_observers_.AddObserver(task_observer);
}

void MessageLoop::RemoveTaskObserver(TaskObserver* task_observer)
{
    DCHECK_EQ(this, current());
    task_observers_.RemoveObserver(task_observer);
}

void MessageLoop::AssertIdle() const
{
    // ֻ���|incoming_queue_|, ��Ϊ�����|work_queue_|����.
    base::AutoLock lock(incoming_queue_lock_);
    DCHECK(incoming_queue_.empty());
}

// ������SEHģʽ��ִ��ѭ��:
// exception_restoration_ = false: ����δ�����쳣������������
// SetUnhandledExceptionFilter()���õĹ�����.
// exception_restoration_ = true: ����δ�����쳣������ѭ��ִ��ǰ�Ĺ�����.
void MessageLoop::RunHandler()
{
    if(exception_restoration_)
    {
        RunInternalInSEHFrame();
        return;
    }

    RunInternal();
}

__declspec(noinline) void MessageLoop::RunInternalInSEHFrame()
{
    LPTOP_LEVEL_EXCEPTION_FILTER current_filter = GetTopSEHFilter();
    __try
    {
        RunInternal();
    }
    __except(SEHFilter(current_filter))
    {
    }
    return;
}

void MessageLoop::RunInternal()
{
    DCHECK(this == current());

    if(state_->dispatcher && type()==TYPE_UI)
    {
        static_cast<base::MessagePumpForUI*>(pump_.get())->
            RunWithDispatcher(this, state_->dispatcher);
        return;
    }

    pump_->Run(this);
}

bool MessageLoop::ProcessNextDelayedNonNestableTask()
{
    if(state_->run_depth != 1)
    {
        return false;
    }

    if(deferred_non_nestable_work_queue_.empty())
    {
        return false;
    }

    Task* task = deferred_non_nestable_work_queue_.front().task;
    deferred_non_nestable_work_queue_.pop();

    RunTask(task);
    return true;
}

void MessageLoop::RunTask(Task* task)
{
    DCHECK(nestable_tasks_allowed_);
    // ִ������, ��ȡ���ϸ�ʽ: ��������.
    nestable_tasks_allowed_ = false;

    HistogramEvent(kTaskRunEvent);
    FOR_EACH_OBSERVER(TaskObserver, task_observers_, WillProcessTask(task));
    task->Run();
    FOR_EACH_OBSERVER(TaskObserver, task_observers_, DidProcessTask(task));
    delete task;

    nestable_tasks_allowed_ = true;
}

bool MessageLoop::DeferOrRunPendingTask(const PendingTask& pending_task)
{
    if(pending_task.nestable || state_->run_depth==1)
    {
        RunTask(pending_task.task);
        // ����ִ����һ������(ע��: �����ܻᵽ��һ��������!).
        return true;
    }

    // ���ڲ���ִ������, ��Ϊ��Ƕ����Ϣѭ��������������Ƕ��ִ��.
    deferred_non_nestable_work_queue_.push(pending_task);
    return false;
}

void MessageLoop::AddToDelayedWorkQueue(const PendingTask& pending_task)
{
    // �Ƶ��ӳ��������. ����delayed_work_queue_����ǰ��ʼ�����. ����ڶ�
    // ��ͬ�ӳ�ִ��ʱ���������������ʱ���õ�, ��֤�Ƚ��ȳ�.
    PendingTask new_pending_task(pending_task);
    new_pending_task.sequence_num = next_sequence_num_++;
    delayed_work_queue_.push(new_pending_task);
}

void MessageLoop::ReloadWorkQueue()
{
    // ��work_queue_Ϊ�յ�ʱ��Ŵ�incoming_queue_��������, �������Լ��ٶ�
    // incoming_queue_��������, �������.
    if(!work_queue_.empty())
    {
        return; // �б�Ҫʱ, �Ŷ�incoming_queue_lock_���������м�������.
    }

    // ���������̳߳�ͻ.
    {
        base::AutoLock lock(incoming_queue_lock_);
        if(incoming_queue_.empty())
        {
            return;
        }
        incoming_queue_.Swap(&work_queue_); // ����ʱ��.
        DCHECK(incoming_queue_.empty());
    }
}

bool MessageLoop::DeletePendingTasks()
{
    bool did_work = !work_queue_.empty();
    while(!work_queue_.empty())
    {
        PendingTask pending_task = work_queue_.front();
        work_queue_.pop();
        if(!pending_task.delayed_run_time.is_null())
        {
            // �ӳ������ɾ��˳�������ɾ��˳�򱣳�һ��. �ӳ�����֮����ܴ���������ϵ.
            AddToDelayedWorkQueue(pending_task);
        }
        else
        {
            delete pending_task.task;
        }
    }
    did_work |= !deferred_non_nestable_work_queue_.empty();
    while(!deferred_non_nestable_work_queue_.empty())
    {
        Task* task = NULL;
        task = deferred_non_nestable_work_queue_.front().task;
        deferred_non_nestable_work_queue_.pop();
        if(task)
        {
            delete task;
        }
    }
    did_work |= !delayed_work_queue_.empty();
    while(!delayed_work_queue_.empty())
    {
        Task* task = delayed_work_queue_.top().task;
        delayed_work_queue_.pop();
        delete task;
    }
    return did_work;
}

// ���ܱ���̨�̵߳���!
void MessageLoop::PostTask_Helper(Task* task, int64 delay_ms, bool nestable)
{
    PendingTask pending_task(task, nestable);

    if(delay_ms > 0)
    {
        pending_task.delayed_run_time =
            base::TimeTicks::Now() + base::TimeDelta::FromMilliseconds(delay_ms);

        if(high_resolution_timer_expiration_.is_null())
        {
            // Windowsʱ�ӵľ�����15.6ms. ���ֻ��С��15.6ms��ʱ�����ø߾���ʱ
            // ��, ��ô18ms��ʱ�Ӵ�Լ����32msʱ����, �������ǳ���. �����Ҫ
            // ������С��2��15.6ms��ʱ�����ø߾���ʱ��. ���Ǿ��Ⱥ͵�Դ����֮��
            // ��ƽ��.
            bool needs_high_res_timers =
                delay_ms < (2 * base::Time::kMinLowResolutionThresholdMs);
            if(needs_high_res_timers)
            {
                base::Time::ActivateHighResolutionTimer(true);
                high_resolution_timer_expiration_ = base::TimeTicks::Now() +
                    base::TimeDelta::FromMilliseconds(kHighResolutionTimerModeLeaseTimeMs);
            }
        }
    }
    else
    {
        DCHECK_EQ(delay_ms, 0) << "delay should not be negative";
    }

    if(!high_resolution_timer_expiration_.is_null())
    {
        if(base::TimeTicks::Now() > high_resolution_timer_expiration_)
        {
            base::Time::ActivateHighResolutionTimer(false);
            high_resolution_timer_expiration_ = base::TimeTicks();
        }
    }

    // ����: ��Ҫ�ڵ�ǰ�߳���ֱ��ѭ��ִ������, �������ܻ�����ⲿ�߳�.
    // �����е����񶼷������.

    scoped_refptr<base::MessagePump> pump;
    {
        base::AutoLock locked(incoming_queue_lock_);

        bool was_empty = incoming_queue_.empty();
        incoming_queue_.push(pending_task);
        if(!was_empty)
        {
            return; // �ڱ�����������Ϣ��.
        }

        pump = pump_;
    }
    // ��Ϊincoming_queue_�п��������ٱ���Ϣѭ��������, ���Ժ������֮ǰ����
    // ���������. ʹ��һ��ջ����������Ϣ��, �������ǿ�����incoming_queue_lock_
    // ���������ScheduleWork.

    pump->ScheduleWork();
}

//------------------------------------------------------------------------------
// Method and data for histogramming events and actions taken by each instance
// on each thread.

void MessageLoop::StartHistogrammer()
{
    if(enable_histogrammer_ && !message_histogram_
        && base::StatisticsRecorder::IsActive())
    {
        DCHECK(!thread_name_.empty());
        message_histogram_ = base::LinearHistogram::FactoryGet(
            "MsgLoop:"+thread_name_,
            kLeastNonZeroMessageId, kMaxMessageId,
            kNumberOfDistinctMessagesDisplayed,
            message_histogram_->kHexRangePrintingFlag);
        message_histogram_->SetRangeDescriptions(event_descriptions_);
    }
}

void MessageLoop::HistogramEvent(int event)
{
    if(message_histogram_)
    {
        message_histogram_->Add(event);
    }
}

bool MessageLoop::DoWork()
{
    if(!nestable_tasks_allowed_)
    {
        // ������������ִ��.
        return false;
    }

    for(;;)
    {
        ReloadWorkQueue();
        if(work_queue_.empty())
        {
            break;
        }

        // ִ�����������.
        do
        {
            PendingTask pending_task = work_queue_.front();
            work_queue_.pop();
            if(!pending_task.delayed_run_time.is_null())
            {
                AddToDelayedWorkQueue(pending_task);
                // �����������������仯, ��Ҫ���µ���ʱ��.
                if(delayed_work_queue_.top().task == pending_task.task)
                {
                    pump_->ScheduleDelayedWork(pending_task.delayed_run_time);
                }
            }
            else
            {
                if(DeferOrRunPendingTask(pending_task))
                {
                    return true;
                }
            }
        } while(!work_queue_.empty());
    }

    // ʲô��û����.
    return false;
}

bool MessageLoop::DoDelayedWork(base::TimeTicks* next_delayed_work_time)
{
    if(!nestable_tasks_allowed_ || delayed_work_queue_.empty())
    {
        recent_time_ = *next_delayed_work_time = base::TimeTicks();
        return false;
    }

    // ���ͺ��ʱ��, ˵���д������������ӳٶ��еȴ�����. Ϊ�����Ч��, ֻ�ǲ���
    // �ص���Time::Now(), �Դ������еȴ����е�����. ����Խ���ͺ�(�д����ȴ�����
    // ������), ����������Խ��Ч.

    base::TimeTicks next_run_time = delayed_work_queue_.top().delayed_run_time;
    if(next_run_time > recent_time_)
    {
        recent_time_ = base::TimeTicks::Now(); // ��Now()������;
        if(next_run_time > recent_time_)
        {
            *next_delayed_work_time = next_run_time;
            return false;
        }
    }

    PendingTask pending_task = delayed_work_queue_.top();
    delayed_work_queue_.pop();

    if(!delayed_work_queue_.empty())
    {
        *next_delayed_work_time = delayed_work_queue_.top().delayed_run_time;
    }

    return DeferOrRunPendingTask(pending_task);
}

bool MessageLoop::DoIdleWork()
{
    if(ProcessNextDelayedNonNestableTask())
    {
        return true;
    }

    if(state_->quit_received)
    {
        pump_->Quit();
    }

    return false;
}

MessageLoop::AutoRunState::AutoRunState(MessageLoop* loop) : loop_(loop)
{
    previous_state_ = loop_->state_;
    if(previous_state_)
    {
        run_depth = previous_state_->run_depth + 1;
    }
    else
    {
        run_depth = 1;
    }
    loop_->state_ = this;

    quit_received = false;
    dispatcher = NULL;
}

MessageLoop::AutoRunState::~AutoRunState()
{
    loop_->state_ = previous_state_;
}

bool MessageLoop::PendingTask::operator<(const PendingTask& other) const
{
    // ��Ϊ���ȼ������������Ԫ�����ȼ����, ����ıȽ���Ҫ������. ʱ����С��
    // ��������.
    if(delayed_run_time < other.delayed_run_time)
    {
        return false;
    }

    if(delayed_run_time > other.delayed_run_time)
    {
        return true;
    }

    // ���ʱ��պ����, ͨ����ž���. �ȶ���ŵĲ�ֵ, ��������֧�����������.
    return (sequence_num - other.sequence_num) > 0;
}


void MessageLoopForUI::DidProcessMessage(const MSG& message)
{
    pump_win()->DidProcessMessage(message);
}

void MessageLoopForUI::AddObserver(Observer* observer)
{
    pump_ui()->AddObserver(observer);
}

void MessageLoopForUI::RemoveObserver(Observer* observer)
{
    pump_ui()->RemoveObserver(observer);
}

void MessageLoopForUI::Run(Dispatcher* dispatcher)
{
    AutoRunState save_state(this);
    state_->dispatcher = dispatcher;
    RunHandler();
}


void MessageLoopForIO::RegisterIOHandler(HANDLE file, IOHandler* handler)
{
    pump_io()->RegisterIOHandler(file, handler);
}

bool MessageLoopForIO::WaitForIOCompletion(DWORD timeout, IOHandler* filter)
{
    return pump_io()->WaitForIOCompletion(timeout, filter);
}