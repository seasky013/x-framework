
#ifndef __base_value_conversionss_h__
#define __base_value_conversionss_h__

#pragma once

// |FilePath|��|Value|������ת������.

class FilePath;
class StringValue;
class Value;

namespace base
{

    // �����߽ӹܷ���ֵ����������.
    StringValue* CreateFilePathValue(const FilePath& in_value);
    bool GetValueAsFilePath(const Value& value, FilePath* file_path);

}

#endif //__base_value_conversionss_h__