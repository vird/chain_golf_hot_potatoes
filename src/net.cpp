#include "net.hpp"
// u32 pseudo_broadcast_limit = 7;
u32 pseudo_broadcast_limit = 1; // DEBUG

void net_ask_con(NetNode &node) {
  try {
    HttpClient c1(node.ip_port);
    Client c(c1);
    Value params;
    params["rev_ip_port"] = gns.node_list[0].ip_port;
    c.CallMethod("handshake", params);
    // cout << c.CallMethod("get_node_list", nullValue) << endl;
    Value res = c.CallMethod("get_node_list", nullValue);
    // не особо проверяем что нам передают, записываем всё
    for(u32 i=0,len=res.size();i<len;i++) {
      NetNode node;
      Value val = res[i];
      node.ip_port = val["ip_port"].asString();
      bool found = false;
      FOR_COL(it, gns.node_list) {
        if (it->ip_port == node.ip_port) {
          found = true;
          break;
        }
      }
      if (!found) {
        gns.node_list.push_back(node);
      }
    }
    
    // cout << "cout bc_height = " << c.CallMethod("bc_height", nullValue) << endl;
    // если мы отстаем, то надо бы блоки довыкачать...
    u32 remote_bc_height = c.CallMethod("bc_height", nullValue).asInt();
    u32 bh = bc_height();
    u32 limit = 40;
    if (!gms.ready && remote_bc_height == bh) {
      gms.ready = true;
    } else {
      while(remote_bc_height > bh && limit--) {
        gms.target_bc_height = remote_bc_height;
        Value param;
        param["id"] = bh;
        // printf("get_block_number %d\n", bh);
        // cout << c.CallMethod("get_block_number", param) << endl;
        Value json_block = c.CallMethod("get_block_number", param);
        Block tmp;
        if (!json_to_block(json_block, tmp)) return;
        if (!block_validate(tmp)) return;
        if (bh == 0) {
          // genesis костыль
          gms_account_new(tmp.header.issuer_pub_key);
          gms.balance[0] = 1e6;
        }
        
        block_apply(tmp);
        bh++;
      }
      return;
    }
    
    {
      // cout << c.CallMethod("get_proposed_block", nullValue) << endl;
      Value json_block = c.CallMethod("get_proposed_block", nullValue);
      Block tmp;
      if (!json_to_block(json_block, tmp)) {
        // а чё с тобой дальше говорить...
        node.is_proposal_valid = false;
        return;
      }
      node.is_proposal_valid = block_validate(tmp);
      if (!node.is_proposal_valid) {
        // не повезло, дуй далі
        return;
      }
      
      proposed_block_replace(tmp);
    }
  } catch (...) {
    // по хорошему плохие ноды надо выбрасывать
    // попробовали 5 раз и до свидания
    // printf("ask bad connection to %s\n", node.ip_port.c_str());
  }
}

void net_broadcast_con(NetNode &node) {
  
  try {
    HttpClient c1(node.ip_port);
    Client c(c1);
    
    Value json_block;
    c.CallMethod("block_push", json_block);
  } catch (...) {
    // по хорошему плохие ноды надо выбрасывать
    // попробовали 5 раз и до свидания
    // printf("broadcast bad connection to %s\n", node.ip_port.c_str());
  }
}

void block_broadcast() {
  for(int i=0;i<pseudo_broadcast_limit;i++) {
    // кто-то ко мне в гости, к кому-то я в гости, так за 7 "рукопожатий" и договоримся.
    gns.broadcast_offset = (gns.broadcast_offset+1)%gns.node_list.size();
    u32 idx = 0;
    FOR_COL(it, gns.node_list) {
      if (it->is_self) continue;
      if (idx++ == gns.broadcast_offset) {
        net_broadcast_con(*it);
        break;
      }
    }
  }
}

void net_tick() {
  // TODO save p2pstate to file
  // printf("net_tick\n");
  // DEBUG
  // printf("node_list:\n");
  // FOR_COL(it, gns.node_list) {
  //   if (it->is_self) continue;
  //   printf("%s\n", it->ip_port.c_str());
  // }
  
  for(int i=0;i<pseudo_broadcast_limit;i++) {
    // кто-то ко мне в гости, к кому-то я в гости, так за 7 "рукопожатий" и договоримся.
    gns.ask_offset = (gns.ask_offset+1)%gns.node_list.size();
    u32 idx = 0;
    FOR_COL(it, gns.node_list) {
      if (it->is_self) continue;
      if (idx++ == gns.ask_offset) {
        net_ask_con(*it);
        break;
      }
    }
  }
  
}