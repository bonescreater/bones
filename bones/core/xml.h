#ifndef BONES_CORE_XML_H_
#define BONES_CORE_XML_H_

namespace rapidxml
{
    template <class ch> class xml_node;
    template <class ch> class xml_attribute;
    template <class ch> class xml_document;
}

namespace bones
{
class XMLAttribute
{
public:
    XMLAttribute(rapidxml::xml_attribute<char> * attr);

    XMLAttribute nextSibling()  const;

    XMLAttribute prevSibling()  const;

    char * name() const;

    size_t nameSize() const;

    char * value() const;

    size_t valueSize() const;

    operator bool()  const;
private:
    rapidxml::xml_attribute<char> * attr_;
};

class XMLNode
{
public:
    XMLNode(rapidxml::xml_node<char> * n);

    XMLNode firstChild() const;

    XMLNode nextSibling() const;

    XMLNode prevSibling() const;

    XMLAttribute firstAttribute() const;

    char * name() const;

    size_t nameSize() const;

    char * value() const;

    size_t valueSize() const;

    operator bool()  const;
private:
    rapidxml::xml_node<char> * node_;
};

class XMLDocument
{
public:
    XMLDocument();

    ~XMLDocument();

    bool parse(char * str);

    XMLNode root() const;
private:
    rapidxml::xml_document<char> * document_;
};

}
#endif