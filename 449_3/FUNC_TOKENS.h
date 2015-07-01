//
//  FUNC_TOKENS.h
//  449_3
//
//  Created by 柳玢 姜 on 14-10-22.
//  Copyright (c) 2014年 LIUBINJIANG. All rights reserved.
//

#ifndef _49_3_FUNC_TOKENS_h
#define _49_3_FUNC_TOKENS_h
#include <vector>
#include <map>
#include <list>

struct evl_token{
    enum token_type {NAME, NUMBER, SINGLE};
    token_type type;
    std::string str;
    int line_no;
};
typedef std::list<evl_token> evl_tokens;

struct evl_statement{
    enum statement_type {MODULE, WIRE, COMPONENT, ENDMODULE};
    statement_type type;
    evl_tokens tokens;
};
typedef std::list<evl_statement> evl_statements;

struct evl_wire{
    std::string name;
    int width;
};
typedef std::vector<evl_wire> evl_wires;

struct evl_pin{
    std::string name;
    int pin_msb;
    int pin_lsb;
};
typedef std::vector<evl_pin> evl_pins;

struct evl_component{
    std::string name;
    std::string type;
    std::vector<evl_pin> evl_pins;
};
typedef std::vector<evl_component> evl_components;

//modified

typedef std::map<std::string, int> evl_wires_table;

evl_wires_table make_wires_table(const evl_wires &wires){
    evl_wires_table wires_table;
    
    //判断wires是不是重复 lec07 p18
    
    for(evl_wires::const_iterator it=wires.begin();it!=wires.end();++it){
        wires_table.insert(std::make_pair(it->name, it->width));
    }
    return wires_table;
}

void display_wires_table(std::ostream &out, const evl_wires_table &wires_table){
    for(evl_wires_table::const_iterator it=wires_table.begin();it!=wires_table.end();++it){
        out<<"wire "<<it->first<<" "<<it->second <<std::endl;
    }
}

//evl_wires_table make_wires_table(const evl_wires &wires)



bool extract_tokens_from_line(std::string line, int line_no, std::list<evl_token> &tokens){
    
    for(size_t i=0;i<line.size();){
        if (line[i] == '/')
        {
            ++i;
            if ((i == line.size()) || (line[i] != '/'))
            {
                std::cerr << "LINE " << line_no << ": a single / is not allowed" <<std::endl;
            }break;
        }
        if (isspace(line[i])){
            ++i; // skip this space character
            continue; // skip the rest of the iteration
        }
        if ((line[i] == '(') || (line[i] == ')') || (line[i] == '[') || (line[i] == ']')
            || (line[i] == ':') || (line[i] == ';') || (line[i] == ',')){
            evl_token token;
            token.type=evl_token::SINGLE;
            token.str=line[i];
            tokens.push_back(token);
            ++i;
            continue;
        }
        if(isalpha(line[i])||line[i]=='_'){
            size_t name_begin = i;
            for (++i; i < line.size(); ++i)
            {
                if (!(isalpha(line[i]) || isdigit(line[i]) || (line[i] == '_') || (line[i] == '$')))
                {
                    break; // [name_begin, i) is the range for the token
                }
            }
            evl_token token;
            token.type=evl_token::NAME;
            token.str=line.substr(name_begin,i-name_begin);
            tokens.push_back(token);
            continue;
        }
        if (isdigit(line[i])){
            size_t number_begin = i;
            for(++i;i<line.size();++i)
            {
                if(!isdigit(line[i]))
                {
                    break;
                }
            }
            evl_token token;
            token.type=evl_token::NUMBER;
            token.str=line.substr(number_begin,i-number_begin);
            tokens.push_back(token);
            continue;
        }
        else{
            std::cerr<<"LINE "<< line_no <<": invalid character" <<std::endl;
            return false;
        }
        
    }
    return true;
}

bool extract_tokens_from_file(std::string file_name,std::list<evl_token> &tokens){
    std::ifstream input_file(file_name.c_str());
    if (!input_file)
    {
        std::cerr << "I can't read " << file_name << "." << std::endl;
        return false;
    }
    tokens.clear();
    std::string line;
    for(int line_no=1;std::getline(input_file, line);++line_no){
        if(!extract_tokens_from_line(line, line_no, tokens)){
            return false;
        }
    }
    
    
    return true;
}

bool move_tokens_to_statement(evl_tokens &statement_tokens, evl_tokens &tokens){
    
    for (; !tokens.empty(); ) {
        statement_tokens.push_back(tokens.front());
        //std::cerr<<"erase "<<tokens.front().str<<std::endl;
        tokens.erase(tokens.begin());
        //std::cerr<<"statement back "<<statement_tokens.back().str<<std::endl;
        if (statement_tokens.back().str==";")
            //std::cerr<<"break from for"<<std::endl;
            break;
    }
    if (statement_tokens.back().str!=";") {
        std::cerr<<"Look for ';' but reach the end of file"<<std::endl;
        return false;
    }
    return true;
}

bool group_tokens_into_statements(evl_statements &statements, evl_tokens &tokens){
    for (; !tokens.empty();) {
        evl_token token=tokens.front();
        //std::cerr<<token.str<<std::endl;
        
        //第一个字符应该是module
        /*if(token.type!=evl_token::NAME){
         std::cerr<<"Need a Name token but found '"<<token.str<<"' on line"<<token.line_no<<std::endl;
         return false;
         }*/
        
        if (token.str=="module") {
            evl_statement module;
            module.type=evl_statement::MODULE;
            
            if(!move_tokens_to_statement(module.tokens, tokens))
                return false;
            statements.push_back(module);
            
            /*for (; !tokens.empty(); ) {
             module.tokens.push_back(tokens.front());
             std::cerr<<"erase "<<tokens.front().str<<std::endl;
             tokens.erase(tokens.begin());
             std::cerr<<"module back "<<module.tokens.back().str<<std::endl;
             if (module.tokens.back().str==";"){
             std::cerr<<"break from for"<<std::endl;
             break;
             }
             else continue;
             
             }*/
        }
        else if (token.str=="endmodule") {
            evl_statement endmodule;
            
            endmodule.type=evl_statement::ENDMODULE;
            endmodule.tokens.push_back(token);
            
            tokens.erase(tokens.begin());
            statements.push_back(endmodule);
            
        }
        else if (token.str=="wire") {
            evl_statement wire;
            wire.type=evl_statement::WIRE;
            if(!move_tokens_to_statement(wire.tokens, tokens))
                return false;
            statements.push_back(wire);
            
        }
        else if(token.str==";"){
            //std::cerr<<"extra ;"<<std::endl;
            tokens.erase(tokens.begin());
        }
        else{
            evl_statement component;
            component.type=evl_statement::COMPONENT;
            if(!move_tokens_to_statement(component.tokens, tokens))
                return false;
            statements.push_back(component);
        }
        
    }
    return true;
}

bool process_component_statement(evl_components &components, evl_statement &s){
    enum state_type{INIT, TYPE, NAME, PINS, PIN_NAME, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE, PINS_DONE, DONE};
    //int bus_width=1;
    state_type state=INIT;
    evl_component component;
    evl_pins pins;
    evl_pin pin;
    pin.pin_msb=-1;
    pin.pin_lsb=-1;
    for(;!s.tokens.empty()&&(state!=DONE);s.tokens.erase(s.tokens.begin())){
        evl_token t=s.tokens.front();
        //std::cerr<<"tokens "<<t.str<<std::endl;
        if(state==INIT){
            if(t.type==evl_token::NAME){
                state=TYPE;
                //std::cerr<<"state: "<<state<<std::endl;
                //component.name=" ";
                component.type=t.str;
            }
            else{
                std::cerr<<"Need NAME but found '"<<t.str<<"' on line"<<t.line_no<<std::endl;
                return false;
            }
            
        }
        else if(state==TYPE){
            if(t.str=="("){
                component.name=" ";
                state=  PINS;
                //std::cerr<<"state: "<<state<<std::endl;
            }
            if(t.type==evl_token::NAME){
                component.name=t.str;
                state=NAME;
                
            }
            
        }
        else if(state==NAME){
            if(t.str=="("){
                state=PINS;
            }
            
            
        }
        else if(state==PINS){
            if(t.type==evl_token::NAME){
                pin.name=t.str;
                pin.pin_msb=-1;
                pin.pin_lsb=-1;
                state=PIN_NAME;
                //std::cerr<<"state: "<<state<<std::endl;
            }
            
        }
        else if(state==PIN_NAME){
            if(t.str==","){
                state=PINS;
                component.evl_pins.push_back(pin);
            }
            else if(t.str=="["){
                state=BUS;
                
            }
            else if(t.str==")"){
                state=PINS_DONE;
                component.evl_pins.push_back(pin);
            }
            
        }
        else if(state==BUS){
            if(t.type==evl_token::NUMBER){
                pin.pin_msb=atoi(t.str.c_str());
                state=BUS_MSB;
            }
            
        }
        else if(state==BUS_MSB){
            if(t.str==":"){
                state=BUS_COLON;
            }
            else if(t.str=="]"){
                //pin.pin_lsb=0;
                state=BUS_DONE;
            }
            else{
                std::cerr<<"Need ':' or ']' but found '"<<t.str<<"' on line"<<t.line_no<<std::endl;
                return false;
            }
        }
        else if(state==BUS_COLON){
            if(t.type==evl_token::NUMBER){
                pin.pin_lsb=atoi(t.str.c_str());
                state=BUS_LSB;
            }
            else{
                std::cerr<<"Need '0' but found '"<<t.str<<"' on line"<<t.line_no<<std::endl;
                return false;
            }
            
            
        }
        else if(state==BUS_LSB){
            if(t.str=="]"){
                state=BUS_DONE;
            }
            else{
                std::cerr<<"Need ']' but found '"<<t.str<<"' on line"<<t.line_no<<std::endl;
                return false;
            }
            
        }
        else if(state==BUS_DONE){
            if(t.str==")"){
                state=PINS_DONE;
                component.evl_pins.push_back(pin);
            }
            if(t.str==","){
                state=PINS;
                component.evl_pins.push_back(pin);
            }
            
        }
        else if(state==PINS_DONE){
            if(t.str==";"){
                state=DONE;
                components.push_back(component);
                //std::cerr<<"state: pinsdone"<<std::endl;
            }
            
        }
        else if(state==DONE){
            if(t.str==";"){
                //components.push_back(component);
                //std::cerr<<"component "<<component.name<<std::endl;
            }
            
            
        }
    }
    if(!s.tokens.empty()||(state!=DONE)){
        std::cerr<<"something wrong with the statement"<<std::endl;
        return false;
    }
    
    return true;
}

bool process_wire_statement(evl_wires &wires, evl_statement &s ){
    enum state_type{INIT, WIRE, DONE, WIRES,WIRE_NAME, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE};
    int bus_width=1;
    
    state_type state= INIT;
    for(;!s.tokens.empty()&&(state!=DONE); s.tokens.erase(s.tokens.begin())){
        evl_token t=s.tokens.front();
        if(state==INIT){
            if(t.str=="wire"){
                state=WIRE;
            }
            else{
                std::cerr<<"Need 'wire' but found '"<<t.str<<"' on line"<<t.line_no<<std::endl;
                return false;
            }
            
        }
        else if(state==WIRE){
            if(t.type==evl_token::NAME){
                evl_wire wire;
                wire.name=t.str;
                wire.width=bus_width;
                wires.push_back(wire);
                //
                std::cerr<<"process wires"<<wire.name<<std::endl;
                
                state=WIRE_NAME;
            }
            else if(t.str=="["){
                state=BUS;
            }
            else{
                std::cerr<<"Need NAME but found '"<<t.str<<"'on line "<<t.line_no<<std::endl;
                return false;
            }
            
        }
        else if(state==WIRES){
            if(t.type==evl_token::NAME){
                evl_wire wire;
                wire.name=t.str;
                wire.width=bus_width;
                wires.push_back(wire);
                
                state=WIRE_NAME;
            }
            
            else{
                std::cerr<<"Need NAME or '[' but found '"<<t.str<<"'on line "<<t.line_no<<std::endl;
                return false;
            }
            
        }
        else if(state==WIRE_NAME){
            if (t.str==",") {
                state=  WIRE;
            }
            else if(t.str==";"){
                state=DONE;
            }
            else{
                std::cerr<<"Need ',' or ';' but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
                return false;
            }
            
        }
        else if(state==BUS){
            if(t.type==evl_token::NUMBER){
                bus_width=atoi(t.str.c_str())+1;
                
                state=BUS_MSB;
            }
        }
        else if(state==BUS_MSB){
            if(t.str==":"){
                state=BUS_COLON;
            }
            else{
                std::cerr<<"Need ':' but found '"<<t.str<<"' on line"<<t.line_no<<std::endl;
                return false;
            }
        }
        else if(state==BUS_COLON){
            if(t.str=="0"){
                state=BUS_LSB;
            }
            else{
                std::cerr<<"Need '0' but found '"<<t.str<<"' on line"<<t.line_no<<std::endl;
                return false;
            }
        }
        else if(state==BUS_LSB){
            if(t.str=="]"){
                state=BUS_DONE;
            }
            else{
                std::cerr<<"Need ']' but found '"<<t.str<<"' on line"<<t.line_no<<std::endl;
                return false;
            }
        }
        else if(state==BUS_DONE){
            if(t.type==evl_token::NAME){
                evl_wire wire;
                wire.name=t.str;
                wire.width=bus_width;
                wires.push_back(wire);
                state=WIRE_NAME;
            }
        }
        else if(state==DONE){
            
        }
    }
    
    if(!s.tokens.empty()||(state!=DONE)){
        std::cerr<<"something wrong with the statement"<<std::endl;
        return false;
    }
    
    return true;
}



void display_tokens(std::ostream &out, const evl_tokens &tokens){
    /*for(int count=0;count<tokens.size();count++){
     std::cout<<tokens[count].str<<std::endl;
     }*/
}

bool store_tokens_to_file(std::string file_name,const std::list<evl_statement> statements){
    std::ofstream output_file(file_name.c_str());
    if(!output_file){
        std::cerr << "I can't write " << file_name << ".syntax ." << std::endl;
        return false;
    }
    
    /*for(int i=0;i<statements.size();i++){
     for(int j=0;j<statements[i].tokens.size();j++){
     output_file << statements[i].tokens[j].str<<" "<<std::endl;
     }
     }*/
    long wire_number=0;
    long component_number=0;
    evl_wires wires;
    evl_components components;
    for(evl_statements::const_iterator iter=statements.begin();iter!=statements.end();++iter){
        
        evl_statement statement=*(iter);
        std::cerr<<(*iter).type<<std::endl;
        if((*iter).type==evl_statement::MODULE){
            evl_tokens module_tokens=(*iter).tokens;
            module_tokens.pop_front();
            
            output_file<<"module "<<module_tokens.front().str<<std::endl;
            }
        else if((*iter).type==evl_statement::WIRE){
            if(!process_wire_statement(wires,statement)){
                return false;
            }
        }
        else if((*iter).type==evl_statement::COMPONENT){
            if(!process_component_statement(components, statement)){
                return false;
            }
            
        }
        else if((*iter).type==evl_statement::ENDMODULE){
            wire_number=wire_number+(wires.end()-wires.begin());
            output_file<<"wires "<<wire_number<<std::endl;
            for(evl_wires::const_iterator i=wires.begin();i!=wires.end();++i){
                output_file<<"wire "<<(*i).name<<" "<<(*i).width<<std::endl;
            }
            
            component_number=components.end()-components.begin();
            output_file<<"components "<<component_number<<std::endl;
            for(evl_components::const_iterator i=components.begin();i!=components.end();++i){
                if((*i).name==" "){
                    output_file<<"component "<<(*i).type<<" "<<(*i).evl_pins.end()-(*i).evl_pins.begin()<<std::endl;
                }else{
                    output_file<<"component "<<(*i).type<<" "<<(*i).name<<" "<<(*i).evl_pins.end()-(*i).evl_pins.begin()<<std::endl;
                }
                for(evl_pins::const_iterator j=(*i).evl_pins.begin();j!=(*i).evl_pins.end();j++){
                    if((*j).pin_msb==-1){
                        output_file<<"pin "<<(*j).name<<std::endl;
                        
                    }
                    else if((*j).pin_lsb==-1){
                        output_file<<"pin "<<(*j).name<<" "<<(*j).pin_msb<<" "<<std::endl;
                    }
                    else{
                        
                        output_file<<"pin "<<(*j).name<<" "<<(*j).pin_msb<<" "<<(*j).pin_lsb<<std::endl;
                    }
                }
            }
            
            break;
            //nothing
        }
    }
    
    return true;
    
    
}
std::string saveWiresComp(const std::list<evl_statement> statements , evl_components &comps,evl_wires &wires){
    long wire_number=0;
    long component_number=0;
    std::string module_name="";
    std::cerr<<"savewirescomp"<<statements.size()<<std::endl;
    for(evl_statements::const_iterator iter=statements.begin();iter!=statements.end();++iter){
        std::cerr<<"saveWiresComp"<<"here"<<std::endl;
        evl_statement statement=*(iter);
        //std::cerr<<(*iter).type<<std::endl;
        if((*iter).type==evl_statement::MODULE){
            evl_tokens module_tokens=(*iter).tokens;
            module_tokens.pop_front();
            module_name=module_tokens.front().str;
        }
        else if((*iter).type==evl_statement::WIRE){
            
            if(!process_wire_statement(wires,statement)){
                
                //return false;
            }
        }
        else if((*iter).type==evl_statement::COMPONENT){
            if(!process_component_statement(comps, statement)){
                //return false;
            }
            
        }
        else if((*iter).type==evl_statement::ENDMODULE){
            wire_number=wire_number+(wires.end()-wires.begin());
            component_number=comps.end()-comps.begin();
            break;
        }
    }
            //nothing
    
    
    return module_name;
}



#endif
