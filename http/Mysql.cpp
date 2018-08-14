//
// Created by cxhmyself on 18-4-20.
//
/*
#include <iostream>
#include "Mysql.h"
#include "../text.h"

#define VEC (vector<std::string>())

用户名唯一标示
void UserMySql::setUseDatas(const std::string &cm) {

    this->init();
    mysql_query( Mysql::getMysql(), cm.c_str());

    MYSQL_RES *res = mysql_store_result(Mysql::getMysql());

    MYSQL_ROW row;

    属性值读入
    MYSQL_FIELD * f ;
    int fl  = 0;
    while( (f = mysql_fetch_field(res) ) ) {
        (*this->field).emplace_back(f->name);
        fl++;
    }

    if(mysql_num_rows(res))
    {
        while( (row = mysql_fetch_row(res)) )
        {
            加入一列
            (*this->Datas).emplace_back(VEC);
            for(int i = 0 ;i < fl ; i++){
                if (row[i] != nullptr ) {
                    std::string mid = row[i];
                    (*this->Datas)[(*(this->Datas)).size() - 1].emplace_back(row[i]);
                }
                else
                    (*this->Datas)[(*(this->Datas )).size()-1].emplace_back("");
            }
        }
    }
}
void UserMySql::init() {
    if(this->Datas != nullptr)
        delete this->Datas;
    if( this->field != nullptr )
        delete this->field;

    this->Datas = new vector<vector<std::string>>();
    this->field = new vector<std::string>();
}
const vector<vector<std::string>>& UserMySql::getConment(const std::string & cm) {
    获取结果 将结果保存成什么样子呢
    this->setUseDatas(cm);
    return *this->Datas;
}
size_t UserMySql::getTableRowsNum(const std::string & str)  {
    select all
    this->getSelect(str , {} );
    return this->Datas->size();
}

using std::string;
static string  beStr(const std::pair<string,string> & p ) {
    string str= p.first;
    str += "'";
    str += p.second;
    str += "'";
    return str;
}

const vector< vector<std::string> > &
UserMySql::getSelect(const std::string &table, const std::initializer_list<std::pair<string ,string>> & condition ,  const std::string& which) {
    进行组装
    std::string command = "select ";
    command += which;
    command += " from ";

    command += table;
    command += " ";
    if(condition.size()) {
        command += " where ";
        auto index= condition.begin();
        for( ; index+1 != condition.end() ;index++ ) {
            command += beStr(*index) + " and ";
        }
        command += beStr(*index) + ";";
    }
    else {
        command += ";";
    }
    std::cout << command << std::endl;
    this->setUseDatas(command);
    return *this->Datas;
}

void UserMySql::getOneCommand(const string & cm){
    this->init();
    mysql_query( Mysql::getMysql(), cm.c_str());
}
void UserMySql::insert(const string & table , const std::initializer_list<string> & li)
{
    string command;
    command += "insert into ";
    command += table;
    command += " values (";
    for(auto x = li.begin() ; x != li.end() ; ){
        command += "'";
        command += *x;
        command += "'";
        x++;
        std::cout << "[command] " << *x << std::endl;
        if(x != li.end())
            command += ",";
    }
    command+=");";
    std::cout << "[command] " << command << std::endl;
    this->getOneCommand(command);
}*/