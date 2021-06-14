#include <string.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <set>
#include <iostream>

using namespace std;




int main(int argc, char** argv)
{
    vector<vector<long>> trans(6,vector<long>(6,0));
    for(int j=0;j<624;j+=2){
        std::string strid = std::to_string(j);
	    std::string name = "../chord/"  + strid + ".txt";
        std::ifstream input(name);
        std::cout<<name<<std::endl;
        long n;
        for(int i=0;i<6;i++){
            for(int k=0;k<6;k++){
            input>>n;
            trans[i][k] = trans[i][k] + n;
            cout<<n<<' ';
            }cout<<endl;
        }cout<<endl;
        input.close();
    }
    
    std::ofstream file;
    file.open("../Trans.txt");
    for(auto u: trans){
        for(auto v: u){
            file<<v<<' ';
        }file<<'\n';
    }    
    file.close();

    return 0;
}
