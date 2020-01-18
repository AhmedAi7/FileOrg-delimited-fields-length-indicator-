#include <iostream>
#include<fstream>
#include<vector>
#include <string.h>
#include <sstream>
#include <stdlib.h>
#include <algorithm>

using namespace std;
struct Course
{
    char ID[6];
    string Name;
    string Instructor;
    short Weeks;
};
struct Primary
{
    char ID [6];
    int Offset;
};
struct Secondary
{
    string Instructor;
    int link;
};
struct List
{
    char ID[6];
    int LinkOfNext;
};
int IndexBinarySearch(vector <Primary>& Prim ,char key[])
{

    int low = 0, high = Prim.size() - 1;
    while (low <= high)
    {
        int middle = (low + high) / 2;
        if (strcmp(Prim[middle].ID, key) == 0)
            return middle;
        else if (strcmp(Prim[middle].ID,key)<0)
            low = middle + 1;
        else
            high = middle - 1;
    }
    return -1;
}
int SecondBinarySearch(vector <Secondary> &Second,string key)
{
    int low = 0, high = Second.size()- 1;
    while (low <= high)
    {
        int middle = (low + high) / 2;
        if (Second[middle].Instructor==key)
            return middle;
        else if (Second[middle].Instructor<key)
            low = middle + 1;
        else
            high = middle - 1;
    }
    return -1;
}
bool compare(const Primary &a,const Primary &b)
{
    return strcmp(a.ID,b.ID)<0;
}
void SortPrim(vector <Primary> &Prim)
{
	Primary temp;
	for (int i = 0; i<Prim.size(); i++)
		for (int j = 0; j<Prim.size()- i; j++)
		{
			if (strcmp(Prim[j].ID,Prim[j + 1].ID)>0)
			{
				temp = Prim[j];
				Prim[j] = Prim[j + 1];
				Prim[j + 1] = temp;
				cout << Prim[i].ID << endl;
			}
		}
}
void Add (fstream &data ,vector <Primary> &Prim ,vector <Secondary> &Second ,vector <List> &inverted);
void UbdateVectors (fstream &data ,vector <Primary> &Prim ,vector <Secondary> &Second ,vector <List> &inverted);
void readprimary(fstream &file,vector <Primary> &Prim);
void readsecondary(fstream &file,vector <Secondary> &Second);
void readlist(fstream &file,vector <List> &invert);
void writeprimary(vector <Primary> &Prim);
void writesecondary(vector <Secondary> &Second);
void writelist(vector <List> &invert);
void PrintCourse(fstream &data,int offset);
void PrintbyPrim(fstream &data,vector <Primary> &Prim,char ID[6]);
vector <List> arrofprimary(Secondary second,vector <List> &invert); // return array of Primary keys for the same Instructor
void PrintInvert(fstream &data,vector <Primary> &Prim,vector<List>& invert);
void DeleteByPrim (fstream &data,vector <Primary> &Prim,char ID [6]);
void UpdateByPrim(fstream &data,vector <Primary> &Prim,char ID [6],vector<Secondary> &Second,vector <List> &invert);
void DeleteBySecond (fstream &data,vector <Primary> &Prim,vector <Secondary> &Second,vector <List> &invert);
void UpdateBySecond (fstream &data,vector <Primary> &Prim,vector <Secondary> &Second,vector <List> &invert);
int main()
{
    // start of Program
    fstream Data("DataFile.txt",ios::in|ios::out|ios::binary);
    fstream Prim("Primary.txt",ios::in|ios::binary);
    fstream Sec("Secondary.txt",ios::in|ios::binary);
    fstream Lis("List.txt",ios::in|ios::binary);
    vector<Primary> vPrim;
    vector<Secondary>vSec;
    vector<List>vLis;
    short flag;
    Data.seekp(0,ios::end);
    if (Data.tellp()>2)
    {
        Data.seekg(0,ios::beg);
        Data.read((char*)&flag,sizeof(flag));
        if (flag == 1)
        {
            UbdateVectors(Data,vPrim,vSec,vLis);
        }
        else
        {
            readprimary(Prim,vPrim);
            readsecondary(Sec,vSec);
            readlist(Lis,vLis);
        }
    }
    // turn flag on
    Data.seekg(0,ios::beg);
    Data.seekp(0,ios::beg);
    flag=1;
    Data.write((char*)&flag,sizeof(flag));
    Prim.close();
    Sec.close();
    Lis.close();
    while (true)
    {
        cout << "1) Add New Course" << endl << "2) Delete Course (By ID)" << endl << "3) Delete Course (By Instructor)" <<endl;
        cout << "4) Print Course (By ID)" << endl << "5) Print Course(By Instructor)" << endl ;
        cout << "6) Update Course (By ID)" << endl << "7) Update Course (By Instructor)" << endl << "8) Exit" << endl;
        int choice;
        cin >> choice;
        cin.ignore();
        if (choice==1)
            Add(Data,vPrim,vSec,vLis);
        else if (choice==2)
        {
            char ID[6];
            cout << "Enter Course ID you want to delete: ";
            cin >> ID;
            DeleteByPrim(Data,vPrim,ID);
        }
        else if (choice==3)
        {
            DeleteBySecond(Data,vPrim,vSec,vLis);
        }
        else if (choice==4)
        {
            char ID[6];
            cout << "Enter Course ID you want to Print: ";
            cin >> ID;
            PrintbyPrim(Data,vPrim,ID);
        }
        else if (choice==5)
        {
            string name;
            cout << "Enter Instructor Name: ";
            cin >> name;
            int IndexInSec=SecondBinarySearch(vSec,name);
            cout << IndexInSec << endl;
            cout << vSec[0].Instructor<<endl;
            if (IndexInSec==-1)
                cout << "Not Found..";
            else
            {
                vector <List>arr=arrofprimary(vSec[IndexInSec],vLis);
                for (int i=0;i<arr.size();i++)
                    PrintbyPrim(Data,vPrim,arr[i].ID);
            }
        }
        else if (choice==6)
        {
            char ID[6];
            cout << "Enter Course ID you want to Update: ";
            cin >> ID;
            cin.ignore();
            UpdateByPrim(Data,vPrim,ID,vSec,vLis);
        }
        else if (choice==7)
        {
            UpdateBySecond(Data,vPrim,vSec,vLis);
        }
        else
        {
            //Save File after Change
            writeprimary(vPrim);
            writesecondary(vSec);
            writelist(vLis);
            flag=0;
            Data.seekp(0,ios::beg);
            Data.write((char*)&flag,sizeof(flag));
            break;
        }
    }
    return 0;
}
void Add (fstream &data ,vector <Primary>&Prim ,vector <Secondary> &Second ,vector <List> &inverted)
{
    Course c;
    // Add record into Data File
    int length;
    cout << "Enter Course Name: ";
    getline(cin,c.Name);
    cin.ignore();
    cout << "Enter Course ID: ";
    cin>> c.ID;
    cin.ignore();
    cout << "Enter Course Instructor Name: ";
    getline(cin,c.Instructor);
    cin.ignore();
    cout << "Enter Course Weeks: ";
    cin>> c.Weeks;
    length=6+c.Instructor.size() +c.Name.size() +4;
    data.clear();
    data.seekp(0,ios::end);
    int offset=data.tellp();
    data.write((char*)&length,sizeof(length));
    data.write(c.ID,sizeof(c.ID));
    data << "|";
    data<< c.Name;
    data << "|";
    data << c.Instructor;
    data << "|";
    data.write((char*)&c.Weeks,sizeof(c.Weeks));
    data << "|";
    data.flush();
    // ----------------------------------------------------
    // Add to Primary index vector
    Primary p;
    for (int i=0 ; i<6; i++)
        p.ID[i]=c.ID[i];
    p.Offset=offset;
    Prim.push_back(p);
    // ---------------------------------------------------
    // Add to Secondary
    int index= SecondBinarySearch(Second,c.Instructor);
    Secondary s;
    List L;
    for (int i=0 ; i<6; i++)
        L.ID[i]=c.ID[i];
    L.LinkOfNext=-1;
    if (index==-1)
    {
        s.Instructor=c.Instructor;
        s.link=inverted.size();
        Secondary tmp;
        for (int i=0; i<Second.size(); i++)
        {
            if(Second[i].Instructor>s.Instructor)
            {
                tmp=Second[i];
                Second[i]=s;
                s=tmp;
            }
        }
        Second.push_back(s);
        inverted.push_back(L);
    }
    else
    {
        int index2=Second[index].link;
        while (true)
        {
            if (inverted[index2].LinkOfNext==-1)
            {
                inverted[index2].LinkOfNext=inverted.size();
                inverted.push_back(L);
                break;
            }
            else
                index2=inverted[index2].LinkOfNext;
        }
    }
//_________________________________________________________________
sort (Prim.begin(),Prim.end(),compare);
}
void PrintCourse(fstream &data,int offset)
{
    int length;
    Course c;
    data.clear();
    data.seekg(offset,ios::beg);
    data.read((char*)&length,sizeof(length));
    data.read(c.ID,sizeof(c.ID));
    data.seekg(1,ios::cur);
    getline(data,c.Name,'|');
    getline(data,c.Instructor,'|');
    data.read((char*)&c.Weeks,sizeof(c.Weeks));
    data.seekg(1,ios::cur);
    cout << "Course ID: "<< c.ID << " Course Name: " << c.Name << " Course Instructor: "<< c.Instructor;
    cout << " Number of Weeks: "<< c.Weeks << endl;
}
void PrintbyPrim(fstream &data,vector <Primary> &Prim,char ID[6])
{
    int index=IndexBinarySearch(Prim,ID);
    if (index==-1)
        cout << "Not Found";
    else
    {
        int offset=Prim[index].Offset;
        PrintCourse(data,offset);
    }
}
void DeleteByPrim (fstream &data,vector <Primary> &Prim,char ID [6])
{
    data.clear();
    int IndexInPrim=IndexBinarySearch(Prim,ID);
    Primary p=Prim[IndexInPrim];
    int offset=(Prim[IndexInPrim].Offset)+4;
    //cout << offset;
    data.seekp(offset,ios::beg);
    //cout << " " << data.tellp()<< " ";
    data<< "*";
    for (int i= IndexInPrim;i<Prim.size()-1;i++)
        Prim[i]=Prim[i+1];
    Prim.pop_back();
    cout << "Done.." << endl;
}

void UpdateByPrim(fstream &data,vector <Primary> &Prim,char ID [6],vector<Secondary> &Second,vector <List> &invert)
{
    // Search on old course
    int IndexInPrim=IndexBinarySearch(Prim,ID);
    if (IndexInPrim==-1)
        cout << "Not Found";
    else {
    // Read new course info
    Course c;
    // Add record into Data File
    int length,lengthofOld;
    cout << "Enter Course Name: ";
    getline(cin,c.Name);
    cin.ignore();
    cout << "Enter Course ID: ";
    cin>> c.ID;
    cin.ignore();
    cout << "Enter Course Instructor Name: ";
    getline(cin,c.Instructor);
    cin.ignore();
    cout << "Enter Course Weeks: ";
    cin>> c.Weeks;
    length=6+c.Instructor.size() +c.Name.size() +4;
    data.clear();

    //save old primary key before delete
    Primary p=Prim[IndexInPrim];
    int offset=p.Offset;

    // check size of old one
    data.seekg(offset,ios::beg);
    data.read ((char*)&lengthofOld,sizeof(lengthofOld));

    DeleteByPrim(data,Prim,ID);
//+4
    if (lengthofOld==length)
        data.seekp(offset,ios::beg);
    else
        data.seekp(0,ios::end);
    int newoffset=data.tellp();
    data.write((char*)&length,sizeof(length));
    data.write(c.ID,sizeof(c.ID));
    data << "|";
    data<< c.Name;
    data << "|";
    data << c.Instructor;
    data << "|";
    data.write((char*)&c.Weeks,sizeof(c.Weeks));
    data << "|";
    data.flush();
    // ----------------------------------------------------
    // Add to Primary index vector
    Primary n;
    for (int i=0 ; i<6; i++)
        n.ID[i]=c.ID[i];
    n.Offset=newoffset;
    Prim.push_back(n);
    // ---------------------------------------------------
    // Add to Secondary
    int index= SecondBinarySearch(Second,c.Instructor);
    Secondary s;
    List L;
    for (int i=0 ; i<6; i++)
        L.ID[i]=c.ID[i];
    L.LinkOfNext=-1;
    if (index==-1)
    {
        s.Instructor=c.Instructor;
        s.link=invert.size();
        Secondary tmp;
        for (int i=0; i<Second.size(); i++)
        {
            if(Second[i].Instructor>s.Instructor)
            {
                tmp=Second[i];
                Second[i]=s;
                s=tmp;
            }
        }
        Second.push_back(s);
        invert.push_back(L);
    }
    else
    {
        int index2=Second[index].link;
        while (true)
        {
            if (invert[index2].LinkOfNext==-1)
            {
                invert[index2].LinkOfNext=invert.size();
                invert.push_back(L);
                break;
            }
            else
                index2=invert[index2].LinkOfNext;
        }
    }
//_________________________________________________________________
sort (Prim.begin(),Prim.end(),compare);
}
}

void DeleteBySecond (fstream &data,vector <Primary> &Prim,vector <Secondary> &Second,vector <List> &invert)
{
    string name;
    cout << "Enter Instructor Name You want to delete: ";
    getline(cin,name);
    cin.ignore();
    int IndexInSecond=SecondBinarySearch(Second,name);
    if (IndexInSecond==-1)
        cout << "Not Found..";
    else
    {
    Secondary S=Second[IndexInSecond];
    for (int i=IndexInSecond;i<Second.size()-1;i++)
        Second[i]=Second[i+1];
    Second.pop_back();
    vector<List> arr = arrofprimary(S,invert);
    for (int i=0;i<arr.size();i++)
    {
        DeleteByPrim(data,Prim,arr[i].ID);
    }
    }
}
void UpdateBySecond (fstream &data,vector <Primary> &Prim,vector <Secondary> &Second,vector <List> &invert)
{
    string name;
    cout << "Enter Instructor Name You want to Update: ";
    getline(cin,name);
    cin.ignore();
    int IndexInSecond=SecondBinarySearch(Second,name);
    if (IndexInSecond==-1)
        cout << "Not Found..";
    else
    {
        vector<List> arr = arrofprimary(Second[IndexInSecond],invert);
        cout << "Instructor You have enter has Course IDs:"<<endl;
        for (int i=0;i<arr.size();i++)
            cout << arr[i].ID << " ";
        cout << endl << "Enter Course ID You want to Update: ";
        char ID[6];
        cin >> ID;
        cin.ignore();
        UpdateByPrim(data,Prim,ID,Second,invert);
    }
}
vector <List> arrofprimary(Secondary second,vector <List> &invert)
{
    vector <List> result;
    int index = second.link;
    while (true)
    {
        result.push_back(invert[index]);
        if (invert[index].LinkOfNext==-1)
            break;
        else
            index=invert[index].LinkOfNext;
    }
return result;
}
void PrintInvert(fstream &data,vector <Primary> &Prim,vector<List>& invert)
{
    for (int i=0;i<invert.size();i++)
    {
        PrintbyPrim(data,Prim,invert[i].ID);
    }
}


void readprimary(fstream &file,vector <Primary> &Prim)
{
    file.seekg(0,ios::beg);
    Primary p,tmp;
    while (!file.fail())
    {
        file.read(p.ID,sizeof(p.ID));
        file.read((char*)&p.Offset,sizeof(p.Offset));
      /*  for (int i=0; i<Prim.size(); i++)
        {
            if(Prim[i].ID>p.ID)
            {
                tmp=Prim[i];
                Prim[i]=p;
                p=tmp;
            }
        }*/
        Prim.push_back(p);
    }
}
void readsecondary(fstream &file,vector <Secondary> &Second)
{
    Secondary s,tmp;
    while (!file.fail())
    {
        getline(file,s.Instructor,'|');
        file.read((char*)&s.link,sizeof(s.link));
        /*for (int i=0; i<Second.size(); i++)
        {
            if(Second[i].Instructor>s.Instructor)
            {
                tmp=Second[i];
                Second[i]=s;
                s=tmp;
            }
        }*/
        Second.push_back(s);
    }
}
void readlist(fstream &file,vector <List> &invert)
{
    List L;
    while (!file.fail())
    {
        file.read(L.ID,sizeof(L.ID));
        file.read((char*)&L.LinkOfNext,sizeof(L.LinkOfNext));
        invert.push_back(L);
    }
}
void writeprimary(vector <Primary> &Prim)
{
    fstream file;
    file.open("Primary.txt",ios::out|ios::binary|ios::trunc);
    for (int i=0; i<Prim.size(); i++)
    {
        file.write((char*)&Prim[i].ID,sizeof(Prim[i].ID));
        file.write((char*)&Prim[i].Offset,sizeof(Prim[i].Offset));
    }
}
void writesecondary(vector <Secondary> &Second)
{
    fstream file;
    file.open("Secondary.txt",ios::out|ios::binary|ios::trunc);
    for (int i=0; i<Second.size(); i++)
    {
        file<<Second[i].Instructor;
        file << '|';
        file.write((char*)&Second[i].link,sizeof(Second[i].link));
    }
}
void writelist(vector <List> &invert)
{
    fstream file;
    file.open("List.txt",ios::out|ios::binary|ios::trunc);
    for (int i=0; i<invert.size(); i++)
    {
        file.write((char*)&invert[i].ID,sizeof(invert[i].ID));
        file.write((char*)&invert[i].LinkOfNext,sizeof(invert[i].LinkOfNext));
    }
}

void UbdateVectors (fstream &data ,vector <Primary> &Prim ,vector <Secondary> &Second ,vector <List> &inverted)
{
    Course c;
    int length,index,index2;
    Primary p,tmp;;
    Secondary s,tmp2;
    List L;
    int offset;
    data.clear();
    data.seekg(2,ios::beg);
    while (!data.fail())
    {
        offset=data.tellg();
        data.read((char*)&length,sizeof(length));
        data.read(c.ID,sizeof(c.ID));
        data.seekg(1,ios::cur);
        getline(data,c.Name,'|');
        getline(data,c.Instructor,'|');
        data.read((char*)&c.Weeks,sizeof(c.Weeks));
        data.seekg(1,ios::cur);
        //check that it define
        if (c.ID[0]=='*')
            continue;
        // Update Primary index vector
        for (int i=0 ; i<6; i++)
            p.ID[i]=c.ID[i];
        p.Offset=offset;
        Prim.push_back(p);
        // ---------------------------------------------------
        // Update Secondary
        index= SecondBinarySearch(Second,c.Instructor);
        for (int i=0 ; i<6; i++)
            L.ID[i]=c.ID[i];
        L.LinkOfNext=-1;
        if (index==-1)
        {
            s.Instructor=c.Instructor;
            s.link=inverted.size();
            for (int i=0; i<Second.size(); i++)
            {
                if(Second[i].Instructor>s.Instructor)
                {
                    tmp2=Second[i];
                    Second[i]=s;
                    s=tmp2;
                }
            }
            Second.push_back(s);
            inverted.push_back(L);
        }
        else
        {
            index2=Second[index].link;
            while (true)
            {
                if (inverted[index2].LinkOfNext==-1)
                {
                    inverted[index2].LinkOfNext=inverted.size();
                    inverted.push_back(L);
                    break;
                }
                else
                    index2=inverted[index2].LinkOfNext;
            }
        }
//_________________________________________________________________
    }
    sort (Prim.begin(),Prim.end(),compare);
}
