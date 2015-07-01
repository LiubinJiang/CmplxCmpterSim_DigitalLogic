//
//  netlist.h
//  449_3
//
//  Created by 柳玢 姜 on 14-10-22.
//  Copyright (c) 2014年 LIUBINJIANG. All rights reserved.
//

#include"FUNC_TOKENS.h"
#include <sstream>
#include <iostream>
#include <map>
#include <fstream>
#include <list>
#include <vector>
#ifndef _49_3_netlist_h
#define _49_3_netlist_h



class netlist;
class gate;
class net;
class pin;
std::string make_net_name(std::string wire_name, int i);

class net{
    friend class netlist;
    std::string net_name;
    //void set(std::string name);
public:std::list<pin *> connections_;
    //bool set(std::string net_name);
public: void append_pin(pin *p);
    
};

class pin{
public:gate *gate_;
public:size_t pin_index_;
//public:net *n_;
public:std::vector<net *> nets_;
    public: bool create(gate *g, size_t pin_index, const evl_pin &p, const std::map<std::string,net *> &nets_table);
    
};

class gate{
public:std::string type;
public:std::string name;
    std::vector<pin *> pins_;
    public: bool create(const evl_component &c, const std::map<std::string,net *> &nets_table, const evl_wires_table &wires_table);
    public: bool create_pin(const evl_pin &ep, size_t pin_index, const std::map<std::string,net *> &nets_table,const evl_wires_table &wires_table);
    
};

class netlist{
    std::list<gate *> gates_;
    std::list<net *> nets_;
    std::map<std::string, net *> nets_table_;
public:void set();
public:void create_net(std::string net_name);
public:bool create_gate(const evl_component &c, const evl_wires_table &wires_table);
public:bool create_gates(const evl_components &comp, const evl_wires_table &wires_table);
public:bool create(const evl_wires &wires, const evl_components &comps, const evl_wires_table &wires_table);
public:bool create_nets(const evl_wires &wires);
public:bool save(std::string nl_file, std::string module_name,netlist &nl);
};




bool netlist::create(const evl_wires &wires, const evl_components &comps, const evl_wires_table &wires_table){
    //
    std::cout<<"netlist::create"<<wires.size()<<std::endl;
    return create_nets(wires)&&create_gates(comps, wires_table);
}

bool netlist::create_gates(const evl_components &comp, const evl_wires_table &wires_table){
    //循环出components里边的component 用来创建gate
    evl_components::const_iterator it;
    
    for(it=comp.begin();it!=comp.end();++it){
        evl_component c=*(it);
        create_gate(c,wires_table);
    }
    return true;
}
bool netlist::create_nets(const evl_wires &wires){
    evl_wires::const_iterator it;
    
    for(it=wires.begin();it!=wires.end();++it){
        evl_wire w=*(it);
        if(w.width==1){
            create_net(w.name);
        }
        else{
            for(int i=0;i<w.width;++i){
                
                create_net(make_net_name(w.name, i));
            }
        }
    }
    return true;
}
bool netlist::create_gate(const evl_component &c, const evl_wires_table &wires_table){
    gate *g=new gate;
    gates_.push_back(g);
    return g->create(c,nets_table_,wires_table);
}

void netlist::create_net(std::string net_name){
    //assert(nets_table_.find(net_name)==nets_table_.end());
    net *n=new net();
    n->net_name=net_name;
    //
    nets_table_[net_name]=n;
    nets_.push_back(n);
}



std::string make_net_name(std::string wire_name, int i){
    //assert(index>=0);
    std::ostringstream oss;
    oss<<wire_name<<"["<<i<<"]";
    return oss.str();
}



bool gate::create(const evl_component &c, const std::map<std::string,net *> &nets_table, const evl_wires_table &wires_table){
    name=c.name;
    type=c.type;
    size_t pin_index=0;
    evl_pins::const_iterator it;
    for(it=c.evl_pins.begin();it!=c.evl_pins.end();++it){
        evl_pin ep=*(it);
        //std::cerr<<"gate::create "<<ep.name<<" pinmsb "<<ep.pin_msb<<" pinlsb "<<ep.pin_lsb<<std::endl;
        create_pin(ep, pin_index,nets_table, wires_table);
        ++pin_index;
    }
    //return validate_structural_semantics();
    return true;
}

bool gate::create_pin(const evl_pin &ep, size_t pin_index, const std::map<std::string,net *> &nets_table,const evl_wires_table &wires_table){
    //resolve semantics of ep uning wires_table
    pin *p =new pin;
    pins_.push_back(p);
    return p->create(this, pin_index,ep,nets_table);
}

bool pin::create(gate *g, size_t pin_index, const evl_pin &p, const std::map<std::string,net *> &nets_table){
    std::string net_name;
    pin_index_=pin_index;
    gate_=g;
    
    //tests
    std::cerr<<net_name<<" "<<p.pin_msb<<std::endl;
    
    
    if(p.pin_msb==-1){
        net_name=p.name;
        std::ostringstream nn;
        nn<<net_name<<"[0]";
        //not net vector yet
        net *n=new net();
        if(nets_table.find(net_name)==nets_table.end()){
            for(int i=0;nets_table.find(nn.str())!=nets_table.end();i++){
                n=nets_table.find(nn.str())->second;
                nets_.push_back(n);
                n->append_pin(this);
                nn.str("");
                nn<<net_name<<"["<<i+1<<"]";
                std::cerr<<nn.str()<<std::endl;
            }
        }else{
            n=nets_table.find(net_name)->second;
            nets_.push_back(n);
            n->append_pin(this);
            }
    }
    
    else{
        int msb=p.pin_msb;
        int lsb=p.pin_lsb;
        std::ostringstream nn;
        if(lsb==-1){
            net_name=p.name;
            std::cerr<<"pin::create "<<net_name<<" "<<msb<<std::endl;
            net *n=new net();
            nn<<net_name<<"["<<msb<<"]";
            n=nets_table.find(nn.str())->second;
            nets_.push_back(n);
            n->append_pin(this);
        }else{
            for(int i=lsb;i<=msb;i++){
                net_name=p.name;
                net *n=new net();
                nn.str("");
                nn<<net_name<<"["<<i<<"]";
                n=nets_table.find(nn.str())->second;
                nets_.push_back(n);
                n->append_pin(this);
            }
        }
    }
    return true;
}

void net::append_pin(pin *p){
    connections_.push_back(p);
}

bool netlist::save(std::string nl_file, std::string module_name,netlist &nl){
    std::ofstream output_file(nl_file.c_str());
    if(!output_file){
        std::cerr << "I can't write " << nl_file << ".netlist ." << std::endl;
        return false;
    }
    output_file<<"module "<<module_name<<std::endl;
    net *n;
    gate *g;
    pin *p;
    std::list<net *>::const_iterator nit;
    std::list<pin *>::const_iterator pit;
    std::vector<pin *>::const_iterator pvit;
    output_file<<"nets "<<nl.nets_.size()<<std::endl;
    for(nit=nl.nets_.begin();nit!=nl.nets_.end();++nit){
        n=*(nit);
        output_file<<"  net "<<n->net_name<<" "<<n->connections_.size()<<std::endl;
        for(pit=n->connections_.begin();pit!=n->connections_.end();++pit){
            p=*(pit);
            if((p->gate_->name)==" "){
            output_file<<"      "<<p->gate_->type<<" "<<p->pin_index_<<std::endl;
            }
            else{
            output_file<<"      "<<p->gate_->type<<" "<<p->gate_->name<<" "<<p->pin_index_<<std::endl;
        }
        }
    }
    
    std::list<gate *>::const_iterator git;
    std::vector<net *>::const_iterator nnit;
    output_file<<"components "<<nl.gates_.size()<<std::endl;
    for(git=nl.gates_.begin();git!=nl.gates_.end();++git){
        g=*(git);
        if(g->name==" "){
            output_file<<"  component "<<g->type<<" "<<g->pins_.size()<<std::endl;
        }else{
            output_file<<"  component "<<g->type<<" "<<g->name<<" "<<g->pins_.size()<<std::endl;
        }
        for(pvit=g->pins_.begin();pvit!=g->pins_.end();++pvit){
            std::string netsnames;
            p=*(pvit);
            for(nnit=p->nets_.begin();nnit!=p->nets_.end();++nnit){
                n=*(nnit);
                netsnames=netsnames+" "+n->net_name;
            }
            output_file<<"      pin "<<p->nets_.size()<<netsnames<<std::endl;
        }
        
    }
    
    return true;
    
}
















#endif
