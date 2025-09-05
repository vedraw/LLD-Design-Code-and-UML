#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <set>
#include <map>
#include <cstdlib>
using namespace std;

enum class Paymode{ UPI, CARD, NETBANKING };
enum class PaymentStatus{ FAILED, SUCCESS };

class Client{
public:
    int id;
    string name;
    set<Paymode> supportedPaymodes;

    Client(int id, string name):id(id), name(name){}
    set<Paymode> getSupportedPaymodes(){
        return supportedPaymodes;
    }
    void addSupportForPaymode(Paymode mode){
        supportedPaymodes.insert(mode);
    }
    void removePaymode(Paymode mode){
        supportedPaymodes.erase(mode);
    }
};

class Bank{
public:
    string name;
    set<Paymode> supportedPaymodes;

    Bank(string name):name(name){}

    PaymentStatus processPayment(int amount, Paymode mode){
        //check if the mode is supported
        cout << "processPayment : bank name " << this->name << " paymode " << static_cast<int>(mode) << endl;
        //return PaymentStatus::SUCCESS;
        int r = rand() % 2;
        if(r==0){
            cout<<"Payment successful"<<endl;
            return PaymentStatus::SUCCESS;
        }
        else{ 
            cout<<"Payment failed"<<endl;
            return PaymentStatus::FAILED;
        }
    }
    void addSupporteForPaymode(Paymode mode){
        supportedPaymodes.insert(mode);
    }
};

class IRoutingsStrategy{
public:
    virtual ~IRoutingsStrategy()=default;
    virtual string route(Paymode mode) = 0;
    virtual void getDistribution() = 0;
};

class WeightedStrategy : public IRoutingsStrategy{
private:
    unordered_map<Paymode, vector<pair<string, int>>> distribution;
public:

    WeightedStrategy(){
        distribution[Paymode::UPI] =  
        {
            {"HDFC", 70},
            {"ICICI", 30}
        };
        distribution[Paymode::CARD] =  
        {
            {"HDFC", 100}
        };
    }

    string route(Paymode mode) override {
        auto &vec = distribution[mode];
        int r = rand() % 100, cum = 0;
        for (auto &p : vec) {
            cum += p.second;
            if (r < cum) return p.first;
        }
        return "HDFC";
    }
    void getDistribution() override {
        cout<<"___"<<endl;
        cout<<"Distribution : "<<endl;
        for(auto it=distribution.begin(); it!=distribution.end(); ++it){
            cout<<"Paymode: " << static_cast<int>(it->first)<<" : ";
            for(auto& pair: it->second){
                cout<<pair.first<<" - "<<pair.second<<endl;
            }
        }
        cout<<"____"<<endl;
    }
};

class PaymentGateway{   
public:
    unordered_map<int, Client> clients;
    unordered_map<string, Bank*> banks;
    set<Paymode> supportedPaymodes;
    IRoutingsStrategy* routingsStrategy;

    PaymentGateway(IRoutingsStrategy* routingsStrategy):routingsStrategy(routingsStrategy){}

    void addClient(int cId, string name){
        clients.emplace(cId, Client(cId, name));
    }
    void removeClient(int cId){
        clients.erase(cId);
    }
    bool hasClient(int cId){
        return clients.count(cId);
    }
    void listSupportedPaymodes(){
        cout<< "supported_modes by PG ";
        for (auto& mode : supportedPaymodes){
            cout << static_cast<int>(mode) << " ";
        }
        cout<<endl;
    }
    void listSupportedPaymodes(int cId){
        set<Paymode> supported_modes=clients.at(cId).getSupportedPaymodes();
        cout<< "supported_modes by client" << cId << " : ";
        for (auto& mode : supported_modes){
            cout << static_cast<int>(mode) << " ";
        }
        cout<<endl;
    }
    void addBank(string bankName, Bank* bank){
        banks.emplace(bankName, bank);
    }
    void addSupportForPaymode(Paymode mode){
        supportedPaymodes.insert(mode);
    }
    void addSupportForPaymode(int cId, Paymode mode){
        clients.at(cId).addSupportForPaymode(mode);
    }
    void removePaymode(Paymode mode){
        supportedPaymodes.erase(mode);
    }
    void removePaymode(int cId, Paymode mode){
        clients.at(cId).removePaymode(mode);
    }
    void showDistribution(){
        routingsStrategy->getDistribution();
    }
    void makePayment(int amount, Paymode mode){
        string bankname = routingsStrategy->route(mode);
        Bank* bank=banks.at(bankname);
        PaymentStatus status = bank->processPayment(amount, mode);
    }
};

int main()
{      
    srand(time(NULL));
    WeightedStrategy ws;
    PaymentGateway pg(&ws);
    pg.addClient(1, "Flipkart");
    pg.hasClient(1);
    pg.hasClient(2);
    pg.addClient(2, "Amazon");

    pg.addSupportForPaymode(Paymode::CARD);
    pg.addSupportForPaymode(1, Paymode::CARD);
    pg.listSupportedPaymodes(1);
    pg.listSupportedPaymodes();

    pg.showDistribution();
    pg.addBank("HDFC", new Bank("HDFC"));
    pg.addBank("ICICI", new Bank("ICICI"));
    pg.makePayment(1000, Paymode::UPI);
    pg.makePayment(5000, Paymode::CARD);
    return 0;
}

