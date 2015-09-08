#include "xml.h"
#include "rapidxml.hpp"
#include "logging.h"

namespace bones
{


XMLAttribute::XMLAttribute(rapidxml::xml_attribute<char> * attr)
    :attr_(attr)
{

}

char * XMLAttribute::name() const
{
    return attr_ ? attr_->name() : nullptr;
}

size_t XMLAttribute::nameSize() const
{
    return attr_ ? attr_->name_size() : 0;
}

char * XMLAttribute::value() const
{
    return attr_ ? attr_->value() : nullptr;
}

size_t XMLAttribute::valueSize() const
{
    return attr_ ? attr_->value_size() : 0;
}

XMLAttribute XMLAttribute::nextSibling()  const
{
    return attr_ ? attr_->next_attribute() : nullptr;
}

XMLAttribute XMLAttribute::prevSibling()  const
{
    return attr_ ? attr_->previous_attribute() : nullptr;
}

XMLAttribute::operator bool()  const
{
    return attr_ != nullptr;
}



XMLNode::XMLNode(rapidxml::xml_node<char> * n)
    :node_(n)
{
}

XMLNode::XMLNode()
: node_(nullptr)
{

}
XMLNode XMLNode::firstChild() const
{
    return node_ ? node_->first_node() : nullptr;
}

XMLNode XMLNode::nextSibling() const
{
    return node_ ? node_->next_sibling() : nullptr;
}

XMLNode XMLNode::prevSibling() const
{
    return node_ ? node_->previous_sibling() : nullptr;
}

XMLAttribute XMLNode::firstAttribute() const
{
    return node_ ? node_->first_attribute() : nullptr;
}

char * XMLNode::name() const
{
    return node_ ? node_->name() : nullptr;
}

size_t XMLNode::nameSize() const
{
    return node_ ? node_->name_size() : 0;
}

char * XMLNode::value() const
{
    return node_ ? node_->value() : nullptr;
}

size_t XMLNode::valueSize() const
{
    return node_ ? node_->value_size() : 0;
}

XMLNode::operator bool()  const
{
    return node_ != nullptr;
}

void XMLNode::acquire(Attribute * attrs, int count) const
{
    if (!attrs || !count)
        return;
    for (auto i = 0; i < count; ++i)
        attrs[i].value = nullptr;

    for (auto i = 0; i < count; ++i)
    {
        if (!attrs[i].name)
            continue;
        auto node_attr = firstAttribute();
        while (node_attr)
        {
            char * name = node_attr.name();

            if (name && !strcmp(attrs[i].name, name))
            {
                attrs[i].value = node_attr.value();
                break;
            }
            node_attr = node_attr.nextSibling();
        }
    }  
}


XMLDocument::XMLDocument()
    :document_(nullptr)
{

}

XMLDocument::~XMLDocument()
{
    if (document_)
        delete document_;
}

bool XMLDocument::parse(char * str)
{
    try
    {
        if (!document_)
            document_ = new rapidxml::xml_document<>;
        document_->parse<0>(str);
    }
    catch (rapidxml::parse_error &)
    {
        assert(0);
        LOG_ERROR << "xml parse fail";
        return false;
    }

    return true;
}

XMLNode XMLDocument::root() const
{
    return document_ ? document_->first_node() : nullptr;
}

XMLDocument::operator bool()  const
{
    return document_ != nullptr;
}

}