#include<iostream>
#include<fstream>
#include<vector>
#include<tuple>
#include<stdio.h>
#include<tgbot/tgbot.h>
//#include<boost/algorithm/string.hpp>

using questBase = std::vector<std::tuple<std::string,std::string,std::string>>;
using questType = std::tuple<std::string,std::string,std::string>;

class Quiz
{
public:
    Quiz()
    {
        fillQuestBase (base);
        baseSize = base.size();
    }

    void fillQuestBase (questBase& questAndAnsw)
    {
     // parse input file
    std::ifstream input ("../data/data.txt");

    std::string line;
    std::string token;
    std::string delimiter{"|"};

    size_t last;
    size_t next;

    for( std::string line; getline( input, line ); )
    {
        last = 0;
        next = 0;

        std::vector<std::string> oneQuest;

        while ((next = line.find(delimiter, last)) != std::string::npos)
            {
                oneQuest.push_back(line.substr(last, next-last));
                last = next + 1;
            }
             
        oneQuest.push_back(line.substr(last));

        // fill the base
        if (oneQuest.size()==3)
        {
            questAndAnsw.push_back(std::make_tuple(oneQuest[0],
                                                   oneQuest[1],
                                                   oneQuest[2]));
        }
    }
}

    questType getCurrQuest()
    {
        if (currentNumb<baseSize)
        {
            return base[currentNumb];
        }

        std::cout<<"End of quiz\n";

        return std::make_tuple("end ", "of ", "quiz\n"); 
    }

    int getBaseSize()
    {
        return baseSize;
    }

    int getScore()
    {
        return score;
    }

    int getNumb()
    {
        return score;
    }

    void increaseScore()
    {
        ++score;
    }

    void incrCurrNumb()
    {
        ++currentNumb;
    }

private:

    questBase base;

    int baseSize = 0;
    int score = 0;
    int currentNumb = 0;
    int attempts = 3;
};


int main() {
    TgBot::Bot bot("PLACE YOUR TOKEN HERE");

    Quiz quiz;
    questType question = quiz.getCurrQuest();
    std::string amount = std::to_string(quiz.getBaseSize());
    int attempts = 2;

    bot.getEvents().onCommand("start", [&bot, &quiz](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Welcome to quiz!\n" + std::get<0>(quiz.getCurrQuest()));
    });

    bot.getEvents().onAnyMessage([&bot, &quiz, &amount, &attempts](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());

        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }

        else if (StringTools::startsWith(message->text, std::get<1>(quiz.getCurrQuest()))) { //boost::algorithm::to_lower(str);
            quiz.increaseScore();
            bot.getApi().sendMessage(message->chat->id, "Rigth, you have "
                                                        + std::to_string(quiz.getScore()) 
                                                        + " score(s) (from "
                                                        + amount
                                                        + ")\n"
                                                        + std::get<2>(quiz.getCurrQuest()));
            quiz.incrCurrNumb();            
            bot.getApi().sendMessage(message->chat->id, std::get<0>(quiz.getCurrQuest()));
        }

        else
        {
            bot.getApi().sendMessage(message->chat->id, "Wrong");
            if (attempts>0) --attempts;
            else 
            {
                attempts = 2;
                bot.getApi().sendMessage(message->chat->id, std::get<2>(quiz.getCurrQuest()));
                quiz.incrCurrNumb();            
                bot.getApi().sendMessage(message->chat->id, std::get<0>(quiz.getCurrQuest()));
            }
        }        
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } 

    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}
