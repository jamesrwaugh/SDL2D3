//
//  KeyValues.h
//  KeyValues
//
//  Created by David S. on 06.06.13.
//  Copyright (c) 2013 David S. All rights reserved.
//

#ifndef KEYVALUES_H
#define KEYVALUES_H

#include <string>
#include <map>

class KeyValue
{
public:
    void LoadFromFile(const std::string &filename);
    void SaveToFile(const std::string &filename);

    void SetString(const std::string &key, const std::string &value);
    void SetInt(const std::string &key, int value);
    void SetFloat(const std::string &key, float value);

    std::string GetString(const std::string &key) const;
    int GetInt(const std::string &key) const;
    float GetFloat(const std::string &key) const;

private:
    void ExtractKey(std::string &key, std::size_t pos, const std::string &line);
    void ExtractValue(std::string &value, std::size_t pos, const std::string &line);
    bool KeyExist(const std::string &key) const;
    void RemoveComment(std::string &line) const;
    bool OnlyWhiteSpace(const std::string &line) const;
    std::map<std::string, std::string> m_Keys;
};

#endif
