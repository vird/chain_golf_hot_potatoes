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
  } catch (...) {
    // по хорошему плохие ноды надо выбрасывать
    // попробовали 5 раз и до свидания
    printf("bad connection to %s\n", node.ip_port.c_str());
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
    printf("bad connection to %s\n", node.ip_port.c_str());
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