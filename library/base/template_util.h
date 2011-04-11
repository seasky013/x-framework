
#ifndef __base_template_util_h__
#define __base_template_util_h__

#pragma once

namespace base
{

    // ����tr1��ģ��.

    template<class T, T v>
    struct integral_constant
    {
        static const T value = v;
        typedef T value_type;
        typedef integral_constant<T, v> type;
    };

    template<class T, T v> const T integral_constant<T, v>::value;

    typedef integral_constant<bool, true> true_type;
    typedef integral_constant<bool, false> false_type;

    template<class T> struct is_pointer : false_type {};
    template<class T> struct is_pointer<T*> : true_type {};

    template<class> struct is_array : public false_type {};
    template<class T, size_t n> struct is_array<T[n]> : public true_type {};
    template<class T> struct is_array<T[]> : public true_type {};

    template<class T> struct is_non_const_reference : false_type {};
    template<class T> struct is_non_const_reference<T&> : true_type {};
    template<class T> struct is_non_const_reference<const T&> : false_type {};

    namespace internal
    {

        // ����YesType��NoTypeȷ��sizeof(YesType) < sizeof(NoType).
        typedef char YesType;

        struct NoType
        {
            YesType dummy[2];
        };

        // ConvertHelperʵ����is_convertible, ���������������ô������. ����
        // ϣ��֪��ϸ�ڵ���: ����������������ͬ�ĺ���, һ����������ΪTo, ��
        // һ�����б�ɵĲ����б�. ���ǵķ���ֵ��С����ͬ, �������ǿ���ʹ��
        // From���͵Ĳ�������, ͨ��sizeof��֪������ѡ��İ汾. ���༼�ɵĸ���
        // ϸ�ڲμ�Alexandrescu��_Modern C++ Design_.
        struct ConvertHelper
        {
            template<typename To>
            static YesType Test(To);

            template<typename To>
            static NoType Test(...);

            template<typename From>
            static From Create();
        };

        // ����ȷ�������Ƿ�Ϊstruct/union/class. Դ��Boost��is_class��type_trait
        // ʵ��.
        struct IsClassHelper
        {
            template<typename C>
            static YesType Test(void(C::*)(void));

            template<typename C>
            static NoType Test(...);
        };

    } //namespace internal

    // ���From����ת��ΪTo, ��true_type�̳�, �����false_type�̳�.
    //
    // ע����������ǿ�ת����, ����ת���Ƿ��������涼���true_type�̳�.
    template<typename From, typename To>
    struct is_convertible
        : integral_constant<bool,
        sizeof(internal::ConvertHelper::Test<To>(
        internal::ConvertHelper::Create<From>())) ==
        sizeof(internal::YesType)> {};

    template<typename T>
    struct is_class
        : integral_constant<bool,
        sizeof(internal::IsClassHelper::Test<T>(0)) ==
        sizeof(internal::YesType)> {};

} //namespace base

#endif //__base_template_util_h__