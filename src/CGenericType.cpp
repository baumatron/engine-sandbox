#include "CGenericType.h"
#include "con_main.h"
#include "m_misc.h"

M_DataTypes CGenericType::GetType() const
{
	return m_type;
}

void CGenericType::SetValue(const bool& value)
{
	if(m_type != t_bool)
	{
		ccout << "Error: mismatched types!\n";
	}
	else
	{
		m_data.m_bool = value;
	}
}
void CGenericType::SetValue(const char& value)
{
	if(m_type != t_char)
	{
		ccout << "Error: mismatched types!\n";
	}
	else
	{
		m_data.m_char = value;
	}
}
void CGenericType::SetValue(const unsigned char& value)
{
	if(m_type != t_uchar)
	{
		ccout << "Error: mismatched types!\n";
	}
	else
	{
		m_data.m_uchar = value;
	}
}
void CGenericType::SetValue(const short& value)
{
	if(m_type != t_short)
	{
		ccout << "Error: mismatched types!\n";
	}
	else
	{
		m_data.m_short = value;
	}
}
void CGenericType::SetValue(const unsigned short& value)
{
	if(m_type != t_ushort)
	{
		ccout << "Error: mismatched types!\n";
	}
	else
	{
		m_data.m_ushort = value;;
	}
}
void CGenericType::SetValue(const long& value)
{
	if(m_type != t_long)
	{
		ccout << "Error: mismatched types!\n";
	}
	else
	{
		m_data.m_long = value;
	}
}
void CGenericType::SetValue(const unsigned long& value)
{
	if(m_type != t_ulong)
	{
		ccout << "Error: mismatched types!\n";
	}
	else
	{
		m_data.m_ulong = value;
	}
}
void CGenericType::SetValue(const float& value)
{
	if(m_type != t_float)
	{
		ccout << "Error: mismatched types!\n";
	}
	else
	{
		m_data.m_float = value;
	}
}
void CGenericType::SetValue(const double& value)
{
	if(m_type != t_double)
	{
		ccout << "Error: mismatched types!\n";
	}
	else
	{
		m_data.m_double = value;
	}
}
void CGenericType::SetValue(const string& value)
{
	if(m_type != t_string)
	{
		ccout << "Error: mismatched types!\n";
	}
	else
	{
		m_string = value;
	}
}






void CGenericType::GetValue(bool& result) const
{
	if(m_type != t_bool)
	{
		ccout << "Error: mismatched types!\n";
		result = 0;
	}
	else
	{
		result = m_data.m_bool;
	}
}
void CGenericType::GetValue(char& result) const
{
	if(m_type != t_char)
	{
		ccout << "Error: mismatched types!\n";
		result = 0;
	}
	else
	{
		result = m_data.m_char;
	}
}
void CGenericType::GetValue(unsigned char& result) const
{
	if(m_type != t_uchar)
	{
		ccout << "Error: mismatched types!\n";
		result = 0;
	}
	else
	{
		result = m_data.m_uchar;
	}
}
void CGenericType::GetValue(short& result) const
{
	if(m_type != t_short)
	{
		ccout << "Error: mismatched types!\n";
		result = 0;
	}
	else
	{
		result = m_data.m_short;
	}
}
void CGenericType::GetValue(unsigned short& result) const
{
	if(m_type != t_ushort)
	{
		ccout << "Error: mismatched types!\n";
		result = 0;
	}
	else
	{
		result = m_data.m_ushort;
	}
}
void CGenericType::GetValue(long& result) const
{
	if(m_type != t_long)
	{
		ccout << "Error: mismatched types!\n";
		result = 0;
	}
	else
	{
		result = m_data.m_long;
	}
}
void CGenericType::GetValue(unsigned long& result) const
{
	if(m_type != t_ulong)
	{
		ccout << "Error: mismatched types!\n";
		result = 0;
	}
	else
	{
		result = m_data.m_ulong;
	}
}
void CGenericType::GetValue(float& result) const
{
	if(m_type != t_float)
	{
		ccout << "Error: mismatched types!\n";
		result = 0;
	}
	else
	{
		result = m_data.m_float;
	}
}
void CGenericType::GetValue(double& result) const
{
	if(m_type != t_double)
	{
		ccout << "Error: mismatched types!\n";
		result = 0;
	}
	else
	{
		result = m_data.m_double;
	}
}
void CGenericType::GetValue(string& result) const
{
	if(m_type != t_string)
	{
		ccout << "Error: mismatched types!\n";
		result = "";
	}
	else
	{
		result = m_string;
	}
}

CGenericType CGenericType::FromString(string data)
{
	CGenericType result(M_DetermineType(data));
	switch(result.GetType())
	{
	case t_bool:
		{
			result.SetValue(M_atob(data));
		}
		break;
	case t_long:
		{
			result.SetValue((long)atoi(data.c_str()));
		}
		break;
	case t_double:
		{
			result.SetValue(atof(data.c_str()));
		}
		break;
	case t_string:
		{
			result.SetValue(data);
		}
		break;
	}
	return result;
}


