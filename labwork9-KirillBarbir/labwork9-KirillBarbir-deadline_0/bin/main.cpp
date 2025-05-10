#include<vector>
#include<functional>
#include<iostream>
#include<map>
#include"lib/Adapter.h"

int main(){
    std::vector<int> v = {1,2,3,4,5,6};
    std::map<int, int> m;
    m[1] = 2;
    m[3] = 4;
    m[5] = 6;
    auto res = keys(m);
    for(int i : res)
        std::cout << i << " ";

}