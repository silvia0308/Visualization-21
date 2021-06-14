#include <string.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <set>
#include <iostream>

using namespace std;


// baryon,forming,star,wind,dark.AGN 0-5
int findtype(int x){
    bool dark,baryon,star,wind,forming,AGN;
    dark = ((x>>1)%2 == 0);
    baryon = ((x>>1)%2 == 1);
    star = ((x>>5)%2 == 1);
    wind = ((x>>6)%2 == 1);
    forming = ((x>>7)%2 == 1);
    AGN = ((x>>8)%2 == 1);
    if(star){
        return 2;
    }
    if(forming){
        return 1;
    }
    if(wind){
        return 3;
    }
    if(AGN){
        return 5;
    }
    if(baryon){
        return 0;
    }
    return 4;
}


void run(int j){

    std::string strid = std::to_string(j);
	std::string name = "../type/"  + strid + ".txt";
    std::string strid2 = std::to_string(j+2);
    std::string name2 = "../type/" + strid2 + ".txt";
    std::ifstream input(name);
    std::ifstream input2(name2);
    std::cout<<name<<std::endl;
    std::vector<pair<int,int>> value,value2;

    int n;
    input>>n;
    int id,x;
    for(int i=0;i<n;i++){
        input>> id >> x;
        value.push_back(make_pair( id, findtype(x) ));
    }
    int m1 = id;

    int n2;
    input2>>n2;
    for(int i=0;i<n2;i++){
        input2>> id >> x;
        value2.push_back(make_pair( id, findtype(x) ));
    }
    input.close();
    input2.close();

    int m2 = id;
    int m = max(m1,m2);
    vector<int> T1(m+1,-1);
    vector<int> T2(m+1,-1);
    for(auto u: value){
        id = u.first;
        x = u.second;
        T1[id] = x;
    }
    for(auto u: value2){
        id = u.first;
        x = u.second;
        T2[id] = x;
    }

    vector<vector<int>> trans(6,vector<int>(6,0));
    for(int i=0;i<m+1;i++){
        int t1 = T1[i];
        int t2 = T2[i];
        if(t1>=0 && t2>=0){
            trans[t1][t2] += 1;
        }
    }
    

    std::ofstream file;
    file.open("../chord/"+ strid +".txt");
    for(auto u: trans){
        for(auto v: u){
            file<<v<<' ';
        }file<<'\n';
    }    
    file.close();
}

int main(int argc, char** argv)
{
  run( std::stoi(argv[1]) );

  return 0;
}
