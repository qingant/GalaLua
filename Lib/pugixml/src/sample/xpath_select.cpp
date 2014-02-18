#include "pugixml.hpp"

#include <iostream>

int main()
{
    pugi::xml_document doc;
    if (!doc.load_file("xgconsole.xml")) return -1;

    doc.print(std::cout);

    pugi::xpath_node_set tools = doc.select_nodes("/Profile/Tools/Tool[@AllowRemote='true' and @DeriveCaptionFrom='lastparam']");
    pugi::xpath_node_set *pnodeset = new pugi::xpath_node_set(tools);
    pugi::xpath_node *xpathnode = (*pnodeset)[0].attribute() ? new pugi::xpath_node((*pnodeset)[0].attribute(),(*pnodeset)[0].node()) : new pugi::xpath_node((*pnodeset)[0].node());
    pugi::xml_node *node = new pugi::xml_node(xpathnode->node().internal_object());
    pugi::xml_node sztemp = node->append_child("xpath_test");
    sztemp.set_name("xpath");
    sztemp.append_child(pugi::node_pcdata).set_value("qqqqqqqqq");
    sztemp.append_attribute("attr")= "attr1111";

    //pugi::xml_node sztemp = tools[0].node().append_child("xpath_test");
    doc.print(std::cout);
}

