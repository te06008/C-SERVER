#ifndef CONTROLER_H

#include <sqlite3.h>
#include <iostream>
#include <cstring>
#include <string.h>
#include "request.h"
#include "httprequestparser.h"
#include "response.h"
#include "httpresponseparser.h"
#include <vector>
#include <sstream>

using namespace std;
using namespace httpparser;

int callback(void *, int, char **, char **);

class UserControler
{
private:
    Request rq;
    sqlite3 *db;
    int rc;
    char *err_msg = 0;
    bool flag=false;
    string id="";
    string name="";
    string age="";

public:
    UserControler(Request request){
        rq = request;
        rc = sqlite3_open("db.sqlite3", &db);
        if(rc){
            cout << "Can't open database \n";
        }
        else{
            //get Id
            vector<string> result = split(rq.uri, '/');
            id = result[2];

            // get Content
            int cnt=0;
            for(int i=0; i<rq.content.size(); i++){
                if(rq.content[i]=='"'){
                    cnt++;
                    if(cnt>0 && cnt%2==0){
                        if(rq.content[i-2]=='g'){
                            for(int j=i+5; (rq.content[j]>='0' && rq.content[j]<='9'); j++){
                                age+=rq.content[j];
                            }
                        }
                        else if(rq.content[i-2]=='m'){
                            for(int j=i+5; (rq.content[j]>='a' && rq.content[j]<='z') || (rq.content[j]>='A' && rq.content[j]<='Z'); j++){
                                name+=rq.content[j];
                            }
                        }
                    }
                }
            }
            if(request.method=="GET"){
                do_get();
            }
            else if(request.method=="PATCH" || request.method=="PUT"){

                do_put();
            }
            else if(request.method=="DELETE"){  
                do_delete();
            }
            else if(request.method=="POST"){
                do_post();
            }
        }

    }

    void do_post(){
        if(name!="" && age!=""){
            string sql = "INSERT INTO user (name, age) VALUES('"+name+"',"+age+")";
            cout << sql << endl;
            rc=sqlite3_exec(db, sql.c_str(), callback, 0, &err_msg);
            cout << err_msg << endl;
        }
    }

    void do_put(){
        if(rq.uri.length()>6 && !flag && !(name=="" && age=="")){
            if(name!=""){
                string sql = "UPDATE user SET Name = '"+name+"' WHERE id = " + id;
                rc=sqlite3_exec(db, sql.c_str(), callback, 0, &err_msg);
            }
            if(age!=""){
                string sql = "UPDATE user SET Age = '"+age+"' WHERE id = " + id;
                rc=sqlite3_exec(db, sql.c_str(), callback, 0, &err_msg);
            }
        }
    }

    void do_get(){
        vector<string> result = split(rq.uri, '/');
        cout << result.size() << endl;
        if(result.size() <= 2){
            rc=sqlite3_exec(db, "SELECT * FROM user", callback, 0, &err_msg);
        }
        else if(result.size() >= 2){
            cout << rq.uri << endl;
            if(!flag){
                string sql = "SELECT * FROM user WHERE id = " + id;
                cout << sql << endl;
                rc=sqlite3_exec(db, sql.c_str(), callback, 0, &err_msg);
            }
            else{
                //올바르지 않은 요청
            }
        }
        else{
            //올바르지 않은 요청
        }
    }

    void do_delete(){
        if(rq.uri.length()>6){
            string sql = "DELETE FROM user WHERE id = " + id;
            rc = sqlite3_exec(db, sql.c_str(), callback, 0, &err_msg);
        }
    }

    vector<string> split(string input, char delimiter) { 
        vector<string> answer;
        stringstream ss(input);
        string temp;
    
        while (getline(ss, temp, delimiter)) {
            answer.push_back(temp);
        }
    
        return answer;
    }
};

int callback(
    void *NotUsed,
    int argc,
    char **argv, 
    char **azColName)
{    
    NotUsed = 0;
    for (int i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}
#endif