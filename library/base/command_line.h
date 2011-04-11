
#ifndef __base_command_line_h__
#define __base_command_line_h__

#pragma once

#include <map>
#include <string>
#include <vector>

class FilePath;

namespace base
{

    // ����������������.
    // ��ѡ���ؿ���ͨ���Ⱥź����һ����ֵ���, ����"-switch=value".
    // ����switchΪǰ׺�Ĳ�������Ϊ���Ӳ���. ��"--"��β�Ĳ�����ֹswitch�Ľ���,
    // ���º����������ݱ���Ϊһ�����Ӳ���.
    // ��һ��ֻ����CommandLine������ʾ��ǰ����������������, ������main()������
    // ��ʼ��(��������ƽ̨��ͬ����).

    class CommandLine
    {
    public:
        // �����в�������
        typedef std::wstring StringType;

        typedef std::vector<StringType> StringVector;
        // map����: ���������Ŀ��ص�key��values
        typedef std::map<std::string, StringType> SwitchMap;

        // ����һ���µĿ�������. |program|�ǽ����еĳ�����(argv[0]).
        enum NoProgram { NO_PROGRAM };
        explicit CommandLine(NoProgram no_program);

        // ͨ��argv[0]�����µ�CommandLine����.
        explicit CommandLine(const FilePath& program);

        ~CommandLine();

        // ��ʼ����ǰ����������CommandLine����. ��Windowsƽ̨���Ե�������
        // ����, ֱ�ӽ���GetCommandLineW(). ��ΪCRT�����������в����ɿ�,
        // �����ǻ�����Ҫ����CommandLineToArgvW����������.
        static void Init(int argc, const char* const* argv);

        // ���ٵ�ǰ���̵�CommandLine����. ����Ҫ���õײ�⵽��ʼ״̬ʱ����(�����ⲿ
        // ���ÿ���Ҫ���������³�ʼ��ʱ). ���Initֻ�����ù�һ��, ������main()������,
        // ����Ҫǿ�Ƶ���Reset().
        static void Reset();

        // CommandLine������ʾ��ǰ���̵�������. ע��: ����ֵ�ǿ��޸ĵ�,
        // ���̰߳�ȫ, �����޸�ʱҪȷ����ȷ��.
        static CommandLine* ForCurrentProcess();

        static CommandLine FromString(const std::wstring& command_line);

        // ���س�ʼ���������ַ���. ����! ������Ҫʹ��, ��Ϊ���ŵ���Ϊ�ǲ���ȷ��.
        StringType command_line_string() const;

        // �����������еĳ�����(��һ���ַ���).
        FilePath GetProgram() const;

        // ��������а���ָ�������򷵻�true.(��������Сд�޹�)
        bool HasSwitch(const std::string& switch_string) const;

        // ����ָ�����ص�ֵ. ���������ֵ���߲�����, ���ؿ��ַ���.
        std::string GetSwitchValueASCII(const std::string& switch_string) const;
        FilePath GetSwitchValuePath(const std::string& switch_string) const;
        StringType GetSwitchValueNative(const std::string& switch_string) const;

        // �õ����̵����п�����.
        size_t GetSwitchCount() const { return switches_.size(); }

        // ��ȡ���еĿ���
        const SwitchMap& GetSwitches() const { return switches_; }

        // Ϊ��������ӿ���[ֵ��ѡ].
        // ����! �ڿ��ؽ�����"--"���������Ч��.
        void AppendSwitch(const std::string& switch_string);
        void AppendSwitchPath(const std::string& switch_string,
            const FilePath& path);
        void AppendSwitchNative(const std::string& switch_string,
            const StringType& value);
        void AppendSwitchASCII(const std::string& switch_string,
            const std::string& value);
        void AppendSwitches(const CommandLine& other);

        // ����һ���������п���ָ���Ŀ���(����ֵ, �������). һ�����������ӽ���.
        void CopySwitchesFrom(const CommandLine& source,
            const char* const switches[], size_t count);

        // ��ȡ���������в���
        const StringVector& args() const { return args_; }

        // ��Ӳ���.
        // ע������: ��Ҫʱ�����ŰѲ����������Ա㱻��ȷ����Ϊһ������.
        // AppendArg��Ҫ����ASCII��; ��ASCII����ᱻ��Ϊ��UTF-8�����ʽ.
        void AppendArg(const std::string& value);
        void AppendArgPath(const FilePath& value);
        void AppendArgNative(const StringType& value);
        void AppendArgs(const CommandLine& other);

        // �����һ�������е����в���. ���|include_program|��true, |other|
        // �ĳ�����Ҳ�ᱻ��ӽ���.
        void AppendArguments(const CommandLine& other, bool include_program);

        // �ڵ�ǰ�����в�������, �����������, ��"valgrind" ���� "gdb --args".
        void PrependWrapper(const StringType& wrapper);

        // ͨ�������������ַ������г�ʼ��, ������������ǵ�һ���ַ���.
        void ParseFromString(const std::wstring& command_line);

    private:
        CommandLine();

        // CommandLine������ʾ��ǰ���̵�������.
        static CommandLine* current_process_commandline_;

        // ������������, �Կո�ָ����������ַ���
        StringType command_line_string_;
        // ������
        StringType program_;

        // �������Ŀ���ֵ��.
        SwitchMap switches_;

        // �ǿ��ص������в���.
        StringVector args_;

        // ���������캯��, ��Ϊ�����´����ǰ���̵������в���ӱ�־λ. ����:
        //     CommandLine cl(*CommandLine::ForCurrentProcess());
        //     cl.AppendSwitch(...);
    };

} //namespace base

#endif //__base_command_line_h__