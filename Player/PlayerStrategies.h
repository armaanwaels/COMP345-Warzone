#ifndef PLAYERSTRATEGIES_H
#define PLAYERSTRATEGIES_H

#include <vector>
#include <string>

class Player;
class Territory;
class Deck;
class Map;

class PlayerStrategy {
public:
    PlayerStrategy();
    PlayerStrategy(const PlayerStrategy& other);
    virtual ~PlayerStrategy();
    PlayerStrategy& operator=(const PlayerStrategy& other);

    virtual std::vector<Territory*> toDefend(Player* player) const = 0;
    virtual std::vector<Territory*> toAttack(Player* player) const = 0;
    virtual bool issueOrder(Player* player, Deck* deck, Map* map) = 0;

    virtual PlayerStrategy* clone() const = 0;
    virtual std::string getStrategyName() const = 0;
};

// Human Strategy
class HumanPlayerStrategy : public PlayerStrategy {
public:
    HumanPlayerStrategy();
    HumanPlayerStrategy(const HumanPlayerStrategy& other);
    ~HumanPlayerStrategy();
    HumanPlayerStrategy& operator=(const HumanPlayerStrategy& other);

    std::vector<Territory*> toDefend(Player* player) const override;
    std::vector<Territory*> toAttack(Player* player) const override;
    bool issueOrder(Player* player, Deck* deck, Map* map) override;

    PlayerStrategy* clone() const override;
    std::string getStrategyName() const override;
};

// Aggressive Strategy
class AggressivePlayerStrategy : public PlayerStrategy {
public:
    AggressivePlayerStrategy();
    AggressivePlayerStrategy(const AggressivePlayerStrategy& other);
    ~AggressivePlayerStrategy();
    AggressivePlayerStrategy& operator=(const AggressivePlayerStrategy& other);

    std::vector<Territory*> toDefend(Player* player) const override;
    std::vector<Territory*> toAttack(Player* player) const override;
    bool issueOrder(Player* player, Deck* deck, Map* map) override;

    PlayerStrategy* clone() const override;
    std::string getStrategyName() const override;
};

// Benevolent Strategy
class BenevolentPlayerStrategy : public PlayerStrategy {
public:
    BenevolentPlayerStrategy();
    BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other);
    ~BenevolentPlayerStrategy();
    BenevolentPlayerStrategy& operator=(const BenevolentPlayerStrategy& other);

    std::vector<Territory*> toDefend(Player* player) const override;
    std::vector<Territory*> toAttack(Player* player) const override;
    bool issueOrder(Player* player, Deck* deck, Map* map) override;

    PlayerStrategy* clone() const override;
    std::string getStrategyName() const override;
};

// Neutral Strategy
class NeutralPlayerStrategy : public PlayerStrategy {
public:
    NeutralPlayerStrategy();
    NeutralPlayerStrategy(const NeutralPlayerStrategy& other);
    ~NeutralPlayerStrategy();
    NeutralPlayerStrategy& operator=(const NeutralPlayerStrategy& other);

    std::vector<Territory*> toDefend(Player* player) const override;
    std::vector<Territory*> toAttack(Player* player) const override;
    bool issueOrder(Player* player, Deck* deck, Map* map) override;

    PlayerStrategy* clone() const override;
    std::string getStrategyName() const override;
};

// Cheater Strategy
class CheaterPlayerStrategy : public PlayerStrategy {
private:
    bool* conqueredThisTurn;

public:
    CheaterPlayerStrategy();
    CheaterPlayerStrategy(const CheaterPlayerStrategy& other);
    ~CheaterPlayerStrategy();
    CheaterPlayerStrategy& operator=(const CheaterPlayerStrategy& other);

    std::vector<Territory*> toDefend(Player* player) const override;
    std::vector<Territory*> toAttack(Player* player) const override;
    bool issueOrder(Player* player, Deck* deck, Map* map) override;

    void resetTurn();

    PlayerStrategy* clone() const override;
    std::string getStrategyName() const override;
};

#endif