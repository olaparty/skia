/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_FUNCTIONCALL
#define SKSL_FUNCTIONCALL

#include "include/private/SkSLDefines.h"
#include "include/sksl/SkSLPosition.h"
#include "src/sksl/ir/SkSLExpression.h"
#include "src/sksl/ir/SkSLType.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace SkSL {

class Context;
class FunctionDeclaration;

/**
 * A function invocation.
 */
class FunctionCall final : public Expression {
public:
    inline static constexpr Kind kExpressionKind = Kind::kFunctionCall;

    FunctionCall(Position pos, const Type* type, const FunctionDeclaration* function,
                 ExpressionArray arguments)
        : INHERITED(pos, kExpressionKind, type)
        , fFunction(*function)
        , fArguments(std::move(arguments)) {}

    // Resolves generic types, performs type conversion on arguments, determines return type, and
    // reports errors via the ErrorReporter.
    static std::unique_ptr<Expression> Convert(const Context& context,
                                               Position pos,
                                               const FunctionDeclaration& function,
                                               ExpressionArray arguments);

    static std::unique_ptr<Expression> Convert(const Context& context,
                                               Position pos,
                                               std::unique_ptr<Expression> functionValue,
                                               ExpressionArray arguments);

    // Creates the function call; reports errors via ASSERT.
    static std::unique_ptr<Expression> Make(const Context& context,
                                            Position pos,
                                            const Type* returnType,
                                            const FunctionDeclaration& function,
                                            ExpressionArray arguments);

    static const FunctionDeclaration* FindBestFunctionForCall(
            const Context& context,
            const std::vector<const FunctionDeclaration*>& functions,
            const ExpressionArray& arguments);

    const FunctionDeclaration& function() const {
        return fFunction;
    }

    ExpressionArray& arguments() {
        return fArguments;
    }

    const ExpressionArray& arguments() const {
        return fArguments;
    }

    bool hasProperty(Property property) const override;

    std::unique_ptr<Expression> clone(Position pos) const override;

    std::string description() const override;

private:
    static CoercionCost CallCost(const Context& context,
                                 const FunctionDeclaration& function,
                                 const ExpressionArray& arguments);

    const FunctionDeclaration& fFunction;
    ExpressionArray fArguments;

    using INHERITED = Expression;
};

}  // namespace SkSL

#endif
