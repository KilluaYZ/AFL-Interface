//
// Created by killuayz on 24-3-21.
//

#include "../../src/utils/json.hpp"
#include <string>
#include <iostream>
using json = nlohmann::json;
#include "../../src/utils/mjson.hpp"
using namespace std;
int main(){
    std::string s = "{\"name\": \"zy\", \"age\": 123}";
    mJson j;
    j.loads(s);
    cout<<j.getString("name")<<endl;
    cout<<j.getInt("age")<<endl;

    j.put("sex", "male");

    mJson nj;
    nj.put("name", "xinxin");
    nj.put("age", 22);
    nj.put("sex", "female");

    j.put("partner", nj);

    for(int i = 0; i < 10; i++){
        mJson tmp_j;
        tmp_j.put("yw", 99);
        tmp_j.put("sx",  98);
        tmp_j.put("cs", 40);
        j.push_back("grades", tmp_j);
    }

    cout<<j.dumps()<<endl;

    auto array = j.getArray("grades");
    for(auto grade : array){
        cout<<"-----------------"<<endl;
        cout<<"yw:"<<grade.getInt("yw")<<endl;
        cout<<"sx:"<<grade.getInt("sx")<<endl;
        cout<<"cs:"<<grade.getInt("cs")<<endl;
    }
}
