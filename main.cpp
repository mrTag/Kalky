#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>

#include "FlatValue.h"
#include "SumValue.h"
#include "ExpressionValue.h"

TEST_CASE("testing FlatValue (+BaseValue)") {
    kalky::FlatValue flatValue;
    flatValue.SetValue(99);
    CHECK(flatValue.GetValue() == 99);

    bool valueUpdatedCalledCorrectly = false;
    flatValue.OnValueChanged.connect([&valueUpdatedCalledCorrectly,&flatValue](kalky::BaseValue *baseValue) {
        if(baseValue == &flatValue)
            valueUpdatedCalledCorrectly = true;
    });
    flatValue.SetValue(101);
    CHECK(valueUpdatedCalledCorrectly);

    kalky::FlatValue* flatValueAsPointer = new kalky::FlatValue();
    bool deleteSignalTriggeredCorrectly = false;
    flatValueAsPointer->OnValueDeleted.connect([&deleteSignalTriggeredCorrectly,flatValueAsPointer](kalky::BaseValue *baseValue) {
        if(flatValueAsPointer == baseValue)
            deleteSignalTriggeredCorrectly = true;
    });
    delete flatValueAsPointer;
    CHECK(deleteSignalTriggeredCorrectly);
}

TEST_CASE("testing SumValue") {
    kalky::SumValue sumValue;
    kalky::FlatValue flatVal1;
    flatVal1.SetValue(1);
    kalky::FlatValue flatVal2;
    flatVal2.SetValue(10);
    kalky::FlatValue* flatVal3 = new kalky::FlatValue();
    flatVal3->SetValue(100);

    sumValue.AddValueToSum(&flatVal1);
    CHECK(sumValue.GetValue() == flatVal1.GetValue());
    bool sumUpdateTriggeredCorrectly = false;
    sumValue.OnValueChanged.connect([&sumUpdateTriggeredCorrectly,&sumValue](kalky::BaseValue *baseValue) {
        if(baseValue == &sumValue)
            sumUpdateTriggeredCorrectly = true;
    });
    sumValue.AddValueToSum(&flatVal2);
    CHECK(sumUpdateTriggeredCorrectly);
    CHECK(sumValue.GetValue() == 11);
    sumUpdateTriggeredCorrectly = false;
    flatVal2.SetValue(20);
    CHECK(sumUpdateTriggeredCorrectly);
    CHECK(sumValue.GetValue() == 21);
    sumValue.AddValueToSum(flatVal3);
    CHECK(sumValue.GetValue() == 121);
    sumUpdateTriggeredCorrectly = false;
    delete flatVal3;
    CHECK(sumUpdateTriggeredCorrectly);
    CHECK(sumValue.GetValue() == 21);
}

TEST_CASE("testing Expression") {
    kalky::Expression expression("sqrt(x^2+y^2)", {"x", "y"}, {2, 0});
    double eval = expression.Evaluate();
    CHECK(eval == 2);
    // begin() and end() simply point to the variable storage,
    // so that variables can be set this way:
    for(auto& var : expression) {
        if(var.Name == "x") {
            var.CurrentValue = 2;
        }
        else if(var.Name == "y") {
            var.CurrentValue = 5;
        }
    }
    double calculatedResult = sqrt(2.0*2.0 + 5.0*5.0);
    eval = expression.Evaluate();
    CHECK(eval == calculatedResult);
    expression.ResetVariablesToDefault();
    eval = expression.Evaluate();
    CHECK(eval == 2);
}

TEST_CASE("testing ExpressionValue") {
    kalky::FlatValue playerDamage;
    playerDamage.SetValue(10);
    kalky::FlatValue playerDefense;
    playerDefense.SetValue(5);
    kalky::ValueCollection playerCollection;
    playerCollection.AddNamedValue("damage", &playerDamage);
    playerCollection.AddNamedValue("defense", &playerDefense);
    kalky::FlatValue playerCriticalMultiplier;
    playerCriticalMultiplier.SetValue(2);

    kalky::FlatValue enemyDamage;
    enemyDamage.SetValue(2);
    kalky::FlatValue enemyDefense;
    enemyDefense.SetValue(1);
    kalky::ValueCollection enemyCollection;
    enemyCollection.AddNamedValue("damage", &enemyDamage);
    enemyCollection.AddNamedValue("defense", &enemyDefense);
    kalky::FlatValue enemyCriticalMultiplier;
    enemyCriticalMultiplier.SetValue(1);

    kalky::Expression damageExpression(
    "(damage - DF_defense) * criticalMultiplier",
    {
        "damage", "DF_defense", "criticalMultiplier"
    },
    {
        0, 0, 1
    });
    kalky::ExpressionValue playerAttacksEnemy(&damageExpression);
    playerAttacksEnemy.AddValueCollection(&playerCollection, "", false);
    playerAttacksEnemy.AddSingleNamedValue("criticalMultiplier", &playerCriticalMultiplier, false);

    kalky::ExpressionValue enemyAttacksPlayer(&damageExpression);
    enemyAttacksPlayer.AddValueCollection(&enemyCollection, "", false);
    enemyAttacksPlayer.AddSingleNamedValue("criticalMultiplier", &enemyCriticalMultiplier, false);

    playerAttacksEnemy.AddValueCollection(&enemyCollection, "DF_", true);
    float playerAttackResult = playerAttacksEnemy.GetValue();
    CHECK(playerAttackResult == 18);

    enemyAttacksPlayer.AddValueCollection(&playerCollection, "DF_", true);
    float enemyAttackResult = enemyAttacksPlayer.GetValue();
    CHECK(enemyAttackResult == -3);

    // the enemy collection should be gone now, so DF_defense is 0
    playerAttackResult = playerAttacksEnemy.GetValue();
    CHECK(playerAttackResult == 20);
    // changing any value that is part of the expression should
    // set the whole ExpressionValue dirty and lead to updated results
    playerDamage.SetValue(20);
    playerAttackResult = playerAttacksEnemy.GetValue();
    CHECK(playerAttackResult == 40);
    playerAttacksEnemy.AddValueCollection(&enemyCollection, "DF_", true);
    enemyDefense.SetValue(20);
    playerAttackResult = playerAttacksEnemy.GetValue();
    CHECK(playerAttackResult == 0);
}
