#include <iostream>
#include "simulation.h"
#include<sstream>
using namespace std;
int main(int argc,char*argv[])
{world_t world;
    if (argc<=3)
    {
        cout<<"Error: Missing arguments!"<<endl;
        return 0;
    }
    world.numSpecies=0;
    world.numCreatures=0;
    int round;
    string v;
    istringstream iStream;
    iStream.str(argv[3]);
    iStream>>round;
iStream.clear();
    if (argc==5) v=argv[4];
    else v='a';


    try
    {
    initWorld(world,argv[1],argv[2]);
        Simulate(world,round,v);
    }
    catch(toolessargument){}

    catch(negativeround){}

    catch(fileopen){}
    catch( toomanyspecies)
    {cout<<"Error: Too many species!"<<endl;
    cout<<"Maximal number of species is "<<MAXSPECIES<<'.'<<endl;}
    catch( toomanyprograms)
    {

    }
    catch( errorinstruction){}
    catch( illegalheight){cout<<"Error: The grid height is illegal!"<<endl;}
    catch( illegalwidth){cout<<"Error: The grid width is illegal!"<<endl;}
    catch( illegalterrain){}
    catch( toomanycreatures)
    {
        cout<<"Error: Too many creatures!"<<endl;
        cout<<"Maximal number of creatures is "<<MAXCREATURES<<'.'<<endl;
    }
    catch( unknownspecies){}
    catch( unknowndir){}
    catch( outsideboundry){}
    catch( wrongability){}
    catch( occupied){}
    catch( lakeerror){};

    return 0;
}