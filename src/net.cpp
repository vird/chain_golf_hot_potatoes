#include "net.hpp"
void net_test_con(NetNode &node) {
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
  } catch (...) {
    // по хорошему плохие ноды надо выбрасывать
    // попробовали 5 раз и до свидания
    printf("bad connection to %s\n", node.ip_port.c_str());
  }
}
void net_tick() {
  // TODO save p2pstate to file
  printf("net_tick\n");
  gns.offset = (gns.offset+1)%gns.node_list.size();
  u32 idx = 0;
  // DEBUG
  printf("node_list:\n");
  FOR_COL(it, gns.node_list) {
    if (it->is_self) continue;
    printf("%s\n", it->ip_port.c_str());
  }
  FOR_COL(it, gns.node_list) {
    if (it->is_self) continue;
    if (idx++ == gns.offset) {
      net_test_con(*it);
      return;
    }
  }
}