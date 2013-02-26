/**  This file is part of MyRearch
 *
 *  Copyright (C) 2011 Ma.Tao <qingant@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */
#ifndef ANY_H
#define ANY_H
#include <typeinfo>

class any
{
public:

    class placeholder // 泛型数据容器holder的非泛型基类
    {
    public:
//虚析构函数，为保证派生类对象能用基类指针析构
        virtual ~placeholder() {}

    public:
        // 提供关于类型的信息
        virtual const std::type_info & type() const = 0;
        virtual placeholder * clone() const = 0;  // 复制
    }; // placeholder

    template<typename ValueType>
    class holder : public placeholder
    {
    public:
      

        holder(const ValueType & value)
                : held(value)
        {

        }
    public:
        virtual const std::type_info & type() const
        {
            // typeid返回std::typeinfo对象引用，后者包含任意对象的类型信息， 如name，此外还提供operator==操作符你可以用typeid(oneObj)==typeid(anotherObj)来比两个对象之类型是否一致。
            return typeid(ValueType);
        }

        virtual placeholder * clone() const
        {
            return new holder(held);  // 改写虚函数，返回自身的复制体
        }
        virtual ~holder()
        {


        }

    public:
        ValueType held; // 数据保存的地方

    }; // holder

// 指向泛型数据容器holder的基类placeholder的指针
    placeholder * content;


//模板构造函数，动态分配数据容器并调用其构造函数

    template<typename ValueType>
    any(ValueType * value)
            : content(new holder<ValueType*>(value))
    {
            
            
    }
    template<typename ValueType>
    any(const ValueType & value)
            : content(new holder<ValueType>(value))
    {}
    any(const any &v)
        :content(v.content->clone())
    {
        
    }
    const std::type_info & type()
    {
        return content->type();
    }
    any & operator=(const any & rhs)
    {
        if (&rhs != this)
        {
            delete content;
            content = rhs.content->clone();        
        }
        return *this;
        
    }


// 与模板构造函数一样，但使用了swap惯用手法
    template<typename ValueType>
    any & operator=(const ValueType & rhs)
    {
// 先创建一个临时对象any(rhs)，再调用下面的swap函数进行底层数据交换，注意与*this交换数据的是临时对象，所以rhs的底层数据并未被更改，只是在swap结束后临时对象拥有了*this的底层数据，而此时*this也拥有了临时对象构造时所拥有的rhs的数据的副本。然后临时对象由于生命期的结束而被自动析构，*this原来的底层数据随之烟消云散。
        any(rhs).swap(*this);
        return *this;
    }

    any & swap(any & rhs) //swap函数，交换底层数据
    {
      placeholder *tmp;
      tmp = content;
      content = rhs.content;
      rhs.content = tmp;
        //std::swap(content, rhs.content); // 只是简单地将两个指针的值互换
        return *this;
    }

    ~any()  //析构函数
    {
        //释放容器，用的是基类指针，这就是placeholder需要一个虚析构函数的原因
        //std::cout<<"typeinfo: "<<this->type().name()<<std::endl;
        delete content;
     
    }
};
template<typename ValueType>
ValueType * any_cast(any * operand)
{
    return  operand && (operand->type()==typeid(ValueType)) ? // #1
            &static_cast<any::holder<ValueType>*>(operand->content)->held
            : 0;
}


template<typename ValueType>
ValueType any_cast(any&  operand)
{
    // 调用any_cast针对指针的版本。
    const ValueType * result = any_cast<ValueType>(&operand);
    //ValueType  result = static_cast<any::holder<ValueType>*>(operand.content)->held;
    if (result==0)
      throw std::bad_exception();
// 如果cast失败，即实际 保存的并非ValueType型数据，则抛出一个异常。
    return *result;
}


#endif // ANY_H
