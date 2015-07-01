//
//  main.cpp
//  449_3
//
//  Created by 柳玢 姜 on 14-10-22.
//  Copyright (c) 2014年 LIUBINJIANG. All rights reserved.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <list>
#include "FUNC_TOKENS.h"
#include "netlist.h"


int main(int argc, const char * argv[])
{
    std::list<evl_token> tokens;
    std::list<evl_statement> statements;
    std::string module_name;
    evl_wires wires;
    evl_components comps;

    if(!extract_tokens_from_file(argv[1],tokens)){
        return -1;
    }
    if(!group_tokens_into_statements(statements, tokens)){
        return -2;
    }
    //
    std::cerr<<"_______________________"<<std::endl;
    
    //MODIFIED
    
    
    //写入 evl_wires, evl_components, module
    module_name=saveWiresComp(statements, comps, wires);
    std::cerr<<"main"<<comps.size()<<std::endl;
    
    evl_wires_table wires_table=make_wires_table(wires);
    
    netlist nl;
    if(!nl.create(wires,comps,wires_table))
        return -1;
    
    std::string nl_file=std::string(argv[1])+".netlist";
    nl.save(nl_file,module_name,nl);
    
    return 0;
    

}




