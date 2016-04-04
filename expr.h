/*
Copyright (c) 2015-2016, Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include "type.h"
#include "variable.h"
#include "ir_node.h"

namespace rl {

class Expr : public Node {
    public:
        enum UB {
            NoUB,
            NullPtr, // NULL ptr dereferencing
            SignOvf, // Signed overflow
            SignOvfMin, // Special case of signed overflow: INT_MIN * (-1)
            ZeroDiv, // FPE
            ShiftRhsNeg, // Shift by negative value
            ShiftRhsLarge, // // Shift by large value
            NegShift, // Shift of negative value
            NoMemeber, // Can't find member of structure
            MaxUB
        };
        Expr (Node::NodeID _id, std::shared_ptr<Data> _value) : Node(_id), value(_value) {}
        Type::TypeID get_type_id () { return value->get_type()->get_type_id (); }
        std::shared_ptr<Data> get_value ();

    protected:
        virtual bool propagate_type () = 0;
        virtual UB propagate_value () = 0;
        std::shared_ptr<Data> value;
};

class VarUseExpr : public Expr {
    public:
        VarUseExpr (std::shared_ptr<Data> _var) : Expr(Node::NodeID::VAR_USE, _var) {}
        void set_value (std::shared_ptr<Data> _new_value);
        std::string emit (std::string offset = "") { return value->get_name (); }

    private:
        bool propagate_type () { return true; }
        UB propagate_value () { return NoUB; }
};

class AssignExpr : public Expr {
    public:
        AssignExpr (std::shared_ptr<Expr> _to, std::shared_ptr<Expr> _from);
        std::string emit (std::string offset = "");

    private:
        bool propagate_type ();
        UB propagate_value ();

        std::shared_ptr<Expr> to;
        std::shared_ptr<Expr> from;
};


class TypeCastExpr : public Expr {
    public:
        TypeCastExpr (std::shared_ptr<Expr> _expr, std::shared_ptr<Type> _type);
        std::string emit (std::string offset = "");

    private:
        bool propagate_type ();
        UB propagate_value ();

        std::shared_ptr<Expr> expr;
        std::shared_ptr<Type> to_type;
        //TODO: add field
//        bool is_implicit;
};

}