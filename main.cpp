#include <QCoreApplication>
#include <QtGlobal>
#include <iostream>
#include <QString>
#include <QTextStream>
#include <QFile>
#include <QStringList>
#include <QVector>
#include <time.h>

using namespace std;

struct Variant
{
    int totalWeigth;
    QVector<int> way;
    Variant():totalWeigth(0) {}
};

typedef QVector<Variant> Otvet;

void printOtvet(Otvet &otvet);
int getMax(Otvet &otvet);
Variant getMaxVariant(Otvet &otvet);

struct Rebro
{
    int from;
    int to;
    int weigth;
};

struct Graph
{
    QVector<Rebro> rebra;
    int numberLeft;
    int maxVershina;
    void add(int from, int to, int weigth);
    void print();
    void load(const QString& fileName);
    void cleanWeigth();
    void distribute();
    void distributeRandom();
    void distributeRandomMinus();
    void calcWays(Variant variant,int vershina, Graph &baseGraph, Otvet &otvet);
    void reshake(const Variant &variant);
    Graph():maxVershina(0) {}
};

void Graph::reshake(const Variant &variant)
{
    int rndRebro=qrand()%rebra.size();
    while ((variant.way.indexOf(rebra[rndRebro].from)==-1) || (variant.way.indexOf(rebra[rndRebro].to)==-1))
        rndRebro=qrand()%rebra.size();
    int points;
    if (rebra[rndRebro].weigth==1) points=1;
    else points=qrand()%(rebra[rndRebro].weigth-1)+1;
    numberLeft=points;
    rebra[rndRebro].weigth+=points;
    distributeRandomMinus();
}


void Graph::calcWays(Variant variant,int vershina, Graph &baseGraph, Otvet &otvet)
{
    variant.way.push_back(vershina);

    if (vershina==maxVershina)
    {
        otvet.push_back(variant);
        return;
    }
    for (int i=0;i<rebra.size();i++)
    {
        if (rebra[i].from==vershina)
        {
            Variant newVariant=variant;

            newVariant.totalWeigth=variant.totalWeigth+((double)baseGraph.rebra[i].weigth/(double)rebra[i].weigth);
            calcWays(newVariant,rebra[i].to,baseGraph,otvet);
        }
    }
}

void Graph::add(int from, int to, int weigth)
{
    Rebro tmp;
    tmp.from=from;
    tmp.to=to;
    tmp.weigth=weigth;
    rebra.push_back(tmp);
    if (to>maxVershina) maxVershina=to;
    if (from>maxVershina) maxVershina=from;
}

void Graph::print()
{
    for (auto i=rebra.begin();i<rebra.end();i++)
        cout << i->from << "-" << i->to << ": " << i->weigth << "\n";
}

void Graph::load(const QString& fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        cout << file.errorString().toStdString() << "\n";
    }
    QTextStream in(&file);
    QString line;
    line = in.readLine();
    numberLeft=line.toInt();
    while(!in.atEnd()) {
        line = in.readLine();
        QStringList list=line.split(" ");

        int from=list[0].toInt();
        for (int i=1;i<list.size();i++)
        {
            QStringList element=list[i].split(":");
            int to=element[0].toInt();
            int weigth=element[1].toInt();
            this->add(from,to,weigth);
        }
    }
    file.close();
}

void Graph::cleanWeigth()
{
    for (auto i=rebra.begin();i<rebra.end();i++)
        i->weigth=0;
}

void Graph::distribute()
{
    for (auto i=0;i<numberLeft;i++)
    {
        rebra[i%rebra.size()].weigth+=1;
    }
    numberLeft=0;
}

void Graph::distributeRandom()
{
    while (numberLeft>0)
    {
        numberLeft--;
        rebra[qrand()%rebra.size()].weigth+=1;
    }
}

void Graph::distributeRandomMinus()
{
    while (numberLeft>0)
    {
        int r=qrand()%rebra.size();
        if (rebra[r].weigth>1)
        {
            numberLeft--;
            rebra[r].weigth--;
        }
    }
}

void printOtvet(Otvet &otvet)
{
    for (int i=0;i<otvet.size();i++)
    {
        cout << i+1 << ") ";
        cout << otvet[i].way[0];
        for (int j=1;j<otvet[i].way.size();j++)
            cout << ", " << otvet[i].way[j];
        cout << " (" << otvet[i].totalWeigth << ")\n";
    }
}

int getMax(Otvet &otvet)
{
    auto eMax=0;
    for (int i=0;i<otvet.size();i++)
    {
        if (otvet[i].totalWeigth>eMax)
            eMax=otvet[i].totalWeigth;
    }
    return eMax;
}

Variant getMaxVariant(Otvet &otvet)
{
    auto eMax=0,eMaxN=0;
    for (int i=0;i<otvet.size();i++)
    {
        if (otvet[i].totalWeigth>eMax)
        {
            eMaxN=i;
            eMax=otvet[i].totalWeigth;
        }
    }
    return otvet[eMaxN];
}

int main()
{
    Graph baseGraph,optimalGraph,newGraph,theBestGraph;
    qsrand(time(NULL));

    baseGraph.load("data.txt");
    //baseGraph.print();

    auto theBest=99999;

    for (int l=0;l<100;l++)
    {
        optimalGraph=baseGraph;
        optimalGraph.cleanWeigth();
        optimalGraph.distribute();

        int bestMax;
        Variant predVariant;

        {
            Otvet otvet;
            Variant tmp;
            optimalGraph.calcWays(tmp,1,baseGraph,otvet);
            predVariant=getMaxVariant(otvet);

            bestMax=getMax(otvet);
        }


        for (int i=0;i<1000;i++)
        {
            Otvet otvet;
            Variant tmp;
            newGraph=optimalGraph;

            newGraph.reshake(predVariant);

            newGraph.calcWays(tmp,1,baseGraph,otvet);

            if (getMax(otvet)<=bestMax)
            {
                bestMax=getMax(otvet);
                optimalGraph=newGraph;
                predVariant=getMaxVariant(otvet);
            }
        }

        {
            Otvet otvet;
            Variant tmp;
            optimalGraph.calcWays(tmp,1,baseGraph,otvet);

            if (getMax(otvet)<theBest)
            {
                theBest=getMax(otvet);
                theBestGraph=optimalGraph;
            }

        }
    }

    {
        Otvet otvet;
        Variant tmp;
        theBestGraph.print();
        theBestGraph.calcWays(tmp,1,baseGraph,otvet);
        printOtvet(otvet);
        cout << "Best max: " << getMax(otvet) << "\n";
    }

    return 0;
}
