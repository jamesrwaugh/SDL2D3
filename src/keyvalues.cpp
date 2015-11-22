//
//  KeyValues.cpp
//  KeyValues
//
//  Created by David S. on 06.06.13.
//  Copyright (c) 2013 David S. All rights reserved.
//

#include <fstream>
#include <iostream>
#include "keyvalues.h"

//---------------------------------------------------------
//---------------------------------------------------------
void KeyValue::LoadFromFile(const std::string &filename)
{
    std::ifstream file;
    file.open(filename.c_str());

    std::string line;
    std::size_t lineNo = 0;
    std::string key, value;

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            lineNo++;
            std::string temp = line;

            temp.erase(0, temp.find_first_not_of("\t "));
            std::size_t pos = temp.find('=');

            RemoveComment(temp);
            if(OnlyWhiteSpace(temp))
                continue;

            ExtractKey(key, pos, temp);
            ExtractValue(value, pos, temp);

            if (!KeyExist(key))
                m_Keys[key] = value;
            else
                std::cerr << key << " already exists." << std::endl;
        }
    }
    else
    {
        std::cerr << "KeyValues: File " << filename << " couldn't be found!\n";
    }
}

//---------------------------------------------------------
//---------------------------------------------------------
void KeyValue::SaveToFile(const std::string &filename)
{
    std::ofstream file;
    file.open(filename.c_str());

    if(file.is_open())
    {
        for (std::map<std::string, std::string>::iterator it = m_Keys.begin(); it != m_Keys.end(); ++it)
        {
            file << it->first << "=" << it->second << std::endl;
        }
    }
    else
    {
        std::cerr << "KeyValues: File " << filename << " couldn't be found!" << std::endl;
    }

    file.close();
}

//---------------------------------------------------------
//---------------------------------------------------------
void KeyValue::SetString(const std::string &key, const std::string &value)
{
    if (!KeyExist(key))
        m_Keys[key] = value;
    else
        std::cerr << key << " already exists." << std::endl;
}

//---------------------------------------------------------
//---------------------------------------------------------
void KeyValue::SetInt(const std::string &key, int value)
{
    if (!KeyExist(key))
        m_Keys[key] = std::to_string(value);
    else
        std::cerr << key << " already exists." << std::endl;
}

//---------------------------------------------------------
//---------------------------------------------------------
void KeyValue::SetFloat(const std::string &key, float value)
{
    if (!KeyExist(key))
        m_Keys[key] = std::to_string(value);
    else
        std::cerr << key << " already exists." << std::endl;
}

//---------------------------------------------------------
//---------------------------------------------------------
std::string KeyValue::GetString(const std::string &key) const
{
    auto it = m_Keys.find(key);
    if (it != m_Keys.end())
        return it->second;
    else
        return "";
}

//---------------------------------------------------------
//---------------------------------------------------------
int KeyValue::GetInt(const std::string &key) const
{
    auto it = m_Keys.find(key);
    if (it != m_Keys.end())
        return std::stoi(it->second);
    else
        return 0;
}

//---------------------------------------------------------
//---------------------------------------------------------
float KeyValue::GetFloat(const std::string &key) const
{
    auto it = m_Keys.find(key);
    if (it != m_Keys.end())
        return std::stof(it->second);
    else
        return 0.0f;
}

//---------------------------------------------------------
//---------------------------------------------------------
void KeyValue::ExtractKey(std::string &key, std::size_t pos, const std::string &line)
{
    key = line.substr(0, pos);
    if (key.find('\t') != line.npos || key.find(' ') != line.npos)
        key.erase(key.find_first_of("\t "));
}

//---------------------------------------------------------
//---------------------------------------------------------
void KeyValue::ExtractValue(std::string &value, std::size_t pos, const std::string &line)
{
    value = line.substr(pos + 1);
    value.erase(0, value.find_first_not_of("\t "));
    value.erase(value.find_last_not_of("\t ") + 1);
}

//---------------------------------------------------------
//---------------------------------------------------------
bool KeyValue::KeyExist(const std::string &key) const
{
    return m_Keys.find(key) != m_Keys.end();
}

//---------------------------------------------------------
//---------------------------------------------------------
void KeyValue::RemoveComment(std::string &line) const
{
    if(line.find(';') != line.npos)
        line.erase(line.find(';'));
}

//---------------------------------------------------------
//---------------------------------------------------------
bool KeyValue::OnlyWhiteSpace(const std::string &line) const
{
    return (line.find_first_not_of(' ') == line.npos);
}
