#include<iostream>
#include<vector>
#include<map>
#include<queue>
#include<fstream>
#include <filesystem>
namespace fs=std::filesystem;
using namespace std;
string SavedFileName;
class Node
{
    public:
    string location;
    int weight;
    bool isBlocked;
    Node *next;
    Node(string l):location(l),weight(0),isBlocked(false),next(NULL){}
    Node(string l,int w,bool b):location(l),weight(w),isBlocked(b),next(NULL){}
};
class Graph
{
    vector<Node *> heads;
    int count;
    public:
    Node *findNode(string name)
    {
        for(Node *n:heads)
        {
            if(n->location==name)
                return n;
        }
        return NULL;
    }
    void createRoadNetwork();
    void findShortestPath(string start,string end);
    void toggleRoadBlock(string s,string d,int w);
    void saveRoadNetwork(string fnm="");
    bool loadRoadNetwork(string filename);
    void displayGraph();
    void clearGraph();
};
void Graph::clearGraph()
{
    for (Node *h:heads) {
        Node *curr=h;
        while (curr!=nullptr) {
            Node *temp=curr;
            curr=curr->next;
            delete temp;
        }
    }
    heads.clear();
    count=0;
}
void Graph::createRoadNetwork()
{
    string src,dest;
    int weight;
    bool blocked;
    cout<<"Enter Number of locations: ";
    cin>>count;
    for(int i=0;i<count;i++)
    {
        cout<<"Enter Location "<<i+1<<": ";
        cin>>src;
        Node *n=findNode(src);
        if(n==NULL)
        {
            n=new Node(src);
            heads.push_back(n);
        }
    }
    for(Node *currHead:heads)
    {
        char choice;
        cout<<"\n---Adding connections for: "<<currHead->location<<"---"<<endl;
        while(true) 
        {
            cout<<"Do you want to add a destination for "<<currHead->location<< "? (y/n): ";
            cin>>choice;
            if(!(choice=='y'||choice=='Y'))break;
            cout<<"Enter Destination Name: ";
            cin>>dest;
            if(dest==currHead->location) 
            {
                cout<<"Error: Self-loops are not allowed. A location cannot connect to itself."<<endl;
                continue;
            }
            cout<<"Enter Distance: ";
            cin>>weight;
            cout<<"Is this route blocked? [1: Yes, 0: No]: ";
            cin>>blocked;
            //Route From Source to Destination
            Node *edge1=new Node(dest);
            edge1->weight=weight;
            edge1->isBlocked=blocked;
            edge1->next=currHead->next;
            currHead->next=edge1;
            //Route From Destination To Source
            Node *destNode=findNode(dest);
            if(destNode!=NULL) 
            {
                Node *edge2=new Node(currHead->location);
                edge2->weight=weight;
                edge2->isBlocked=blocked;
                edge2->next=destNode->next;
                destNode->next=edge2;
            }
            cout<<"Connection added: "<<currHead->location<<" <-> "<<dest<<(blocked?" [BLOCKED] ":"")<<endl;
        }
    }
}
void Graph::findShortestPath(string start,string end)
{
    map<string,int> dist;
    map<string,string> parent;
    priority_queue<pair<int,string>,vector<pair<int,string>>,greater<pair<int,string>>> pq;
    for(Node *h:heads)
        dist[h->location]=numeric_limits<int>::max();
    if(dist.find(start)==dist.end())
    {
        cout<<"Start location not found!"<<endl;
        return;
    }
    dist[start]=0;
    pq.push({0,start});
    while(!pq.empty())
    {
        string u=pq.top().second;
        int d=pq.top().first;
        pq.pop();
        if(d>dist[u])continue;
        if(u==end)break;
        Node *headNode=findNode(u);
        Node *edge=headNode->next;
        while(edge!=NULL)
        {
            if(edge->isBlocked)
            {
                edge=edge->next;
                continue;
            }
            if(dist[u]+edge->weight<dist[edge->location])
            {
                dist[edge->location]=dist[u]+edge->weight;
                parent[edge->location]=u;
                pq.push({dist[edge->location],edge->location});
            }
            edge=edge->next;
        }
    }
    if(dist[end]==numeric_limits<int>::max())
        cout<<"\nNo available route from "<<start<<" to "<<end<<" (Roads may be blocked)."<<endl;
    else
    {
        cout<<"\nShortest Distance: "<<dist[end]<<"km"<<endl;
        cout<<"Optimized Path: ";
        vector<string> path;
        for(string v=end;v!="";v=parent[v]){
            path.push_back(v);
            if(v==start)break;
        }
        for(int i=path.size()-1;i>=0;i--){
            cout<<path[i]<<(i==0?"":" -> ");
        }
        cout<<endl;
    }
}
void Graph::toggleRoadBlock(string s,string d,int w)
{
    Node *srcNode=findNode(s);
    Node *destNode=findNode(d);
    if(srcNode==NULL||destNode==NULL) 
    {
        cout<<"Error: One or both locations do not exist!"<<endl;
        return;
    }
    bool found=false;
    //Source->Destination
    Node *curr=srcNode->next; 
    while(curr!=NULL)
    {
        if(curr->location==d&&curr->weight==w)
        {
            curr->isBlocked=!curr->isBlocked;
            found=true;
            break;
        }
        curr=curr->next;
    }
    //Destination->Source
    curr=destNode->next;
    while(curr!=NULL)
    {
        if (curr->location==s&&curr->weight==w)
        {
            curr->isBlocked=!curr->isBlocked;
            break;
        }
        curr=curr->next;
    }
    if(found)
        cout<<"Status of road "<<s<<" <-> "<<d<<" ("<<w<<"km) : "<<((curr->isBlocked)?" [BLOCKED] ":" [OPEN] ")<<endl;
    else
        cout<<"No direct road found between "<<s<<" and "<<d<<endl;
}
string getNextFileName()
{
    int i=0;
    while(true)
    {
        string name="unamedNetwork"+to_string(i);
        ifstream file(name);
        if(!file.is_open())
            return name;
        ++i;
    }
}
void Graph::saveRoadNetwork(string fnm)
{
    ofstream newfile;
    if(fnm=="")
        newfile.open(getNextFileName());
    else
        newfile.open(fnm);
    if(newfile.is_open())
    {
        int x=0;
        for(Node *h:heads)
        {
            Node *curr=h;
            newfile<<"Head "<<x<<endl<<"|";
            while(curr!=NULL)
            {
                newfile<<curr->location<<","<<curr->weight<<","<<curr->isBlocked<<","<<((curr->next!=NULL)?curr->next->location:"NULL")<<"|";
                newfile<<((curr->next!=NULL)?"|":"\n");
                curr=curr->next;
            }
            ++x;
        }
        newfile.close();
    }
    else
        cout<<endl<<"Unable To Open File!"<<endl;
}
bool Graph::loadRoadNetwork(string fname)
{
    ifstream file(fname);
    if(!file.is_open())
    {
        cout<<"ERROR: Could not open file!"<<endl;
        return false;
    }
    SavedFileName=fname;
    heads.clear();
    string line;
    while (getline(file, line))
    {
        // Skipping "Head X" lines or lines that don't contain data
        if(line.find("Head")!=string::npos||line.empty())
            continue; 
        stringstream ss(line);
        string segment;
        Node *currentHead=nullptr;
        // Split by the double pipe ||
        while (getline(ss, segment, '|'))
        {
            if (segment.empty()) continue; // Skip empty segments between ||
            // Remove leading/trailing pipes if the split left any
            stringstream edgeStream(segment);
            string loc, weightStr, blockedStr, nextLoc;
            getline(edgeStream, loc, ',');
            getline(edgeStream, weightStr, ',');
            getline(edgeStream, blockedStr, ',');
            getline(edgeStream, nextLoc, ',');
            // Logic to rebuild the nodes
            Node *newNode=new Node(loc,stoi(weightStr),(blockedStr == "1"));
            if (currentHead==nullptr) 
            {
                // This is the first node in the sequence (the Head)
                currentHead=newNode;
                heads.push_back(currentHead);
            } 
            else 
            {
                // Link it to the previous node in this list
                Node *temp=currentHead;
                while(temp->next!=nullptr) temp=temp->next;
                temp->next=newNode;
            }
        }
    }
    file.close();
    cout<<"Network successfully loaded into heads vector!"<<endl;
    return true;
}
void Graph::displayGraph()
{
    cout<<"\n--- Road Network (Adjacency List) ---"<<endl;
    for(Node *h:heads)
    {
        cout<<h->location<<" is connected to:"<<endl;
        Node *t=h->next;
        if(!t)
            cout<<"  (No outgoing roads)"<<endl;
        while(t!=NULL)
        {
            cout<<"  -> "<<t->location<<" [Distance: "<<t->weight<<"km]";
            if(t->isBlocked)
                cout<<" **RESTRICTED**";
            cout<<endl;
            t=t->next;
        }
        cout<<"------------------------------------"<<endl;
    }
}
bool showSavedFiles()
{
    cout<<"\n---Scanning for Saved Networks---\n";
    bool found=false;
    for(const auto &entry:fs::directory_iterator("."))
    {
        if(entry.is_regular_file())
        {
            cout<<"  [FILE] "<<entry.path().filename().string()<<std::endl;
            found=true;
        }
    }
    if(found)
        return true;
    else
    {
        cout<<"ERROR: No files found."<<endl;
        return false;
    }
}
void showNavigationsOperationsMenu(Graph &g1)
{
    string s,e;
    int choice;
    while(true)
    {
        cout<<"\n-----------------Navigation Operations Menu-----------------";
        cout<<endl<<"1. Display Network"<<endl<<"2. Find Shortest Path"<<endl<<"3. Toggle Road Status"<<endl<<"4. Recreate Road Network"<<endl<<"5. Extend Road Network"<<endl<<"6. Return"<<endl;
        cout<<endl<<"Enter Your Choice: ";
        cin>>choice;
        switch(choice)
        {
            case 1: g1.displayGraph();
                    break;
            case 2: cout<<"Enter Source: ";
                    cin>>s;
                    cout<<"Enter Destination: ";
                    cin>>e;
                    g1.findShortestPath(s,e);
                    break;
            case 3: cout<<"Enter Source: ";
                    cin>>s;
                    cout<<"Enter Destination: ";
                    cin>>e;
                    cout<<"Enter Weight: ";
                    cin>>choice;
                    g1.toggleRoadBlock(s,e,choice);
                    break;
            case 4: g1.clearGraph();
                    g1.createRoadNetwork();
                    break;
            case 5: g1.createRoadNetwork();
            case 6: break;
            default:cout<<"Invalid Choice!"<<endl;
        }
        if(choice==6)break;
    } 
}
int main()
{
    Graph g1;
    string fname;
    int choice;
    while(true)
    {
        cout<<"------------------------------MENU------------------------------[TESTING]"<<endl;
        cout<<"1. Create Road Network"<<endl<<"2. Use Existing Road Network"<<endl<<"3. Exit"<<endl;
        cout<<"Enter Your Choice: ";
        cin>>choice;
        switch(choice)
        {
            case 1: g1.clearGraph();
                    g1.createRoadNetwork();
                    showNavigationsOperationsMenu(g1);
                    g1.saveRoadNetwork();
                    break;
            case 2: if(showSavedFiles())
                    {
                        cout<<"Enter File Name: ";
                        cin>>fname;
                        if(g1.loadRoadNetwork(fname))
                        {
                            showNavigationsOperationsMenu(g1);
                            g1.saveRoadNetwork(SavedFileName);
                        }
                    }
                    break;
            case 3: return 0;
            default:cout<<"Invalid Choice!"<<endl;
        }
    }
}