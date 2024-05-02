#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <unistd.h>
using namespace std;

class pai
{
public:
    int a = 0;
    int midd;
    int box[1000];
    int changdu;
    int *m=box;
    int quick_pai_once(int*m,int i, int j)
    {
        int base=m[i];
        while (i < j)
        {
            while (base<m[j] && i<j)
            {
                j--;
            }
            if (base>m[j])
            {
                swap(m[i++], m[j]);
            }
            
            while (m[i]<base && i<j)
            {
                i++;
            }
            if (m[i]>base)
            {
                swap(m[i], m[j--]);
            }
        }
        return i;
    }
    int quick_pai(int*m,int i, int j)
    {
        if (i < j)
        {
            midd=quick_pai_once(m,i,j);
            quick_pai(m,i,midd-1);
            quick_pai(m,midd+1,j);
        }
        
    }
    int printf_box()
    {
        cout <<"别吵，我在思考"<<endl;
        sleep(2);
        cout << "简见证奇迹的时刻" << endl;
        for (a = 0; a < changdu+1; a++)
        {
            cout << box[a] << " ";
        }
    }
};
int main()
{
    pai have_a_try;
    int n;
    cout << "你现在可以输入你想排序的数字啦(请你用51666结束因为五一让我很快乐=o=)" << endl;
    for (n = 0; n < 10000; n++)
    {
        cin >> have_a_try.box[n];
        if (have_a_try.box[n] == 51666)
        {
            n-=1;
            break;
        }
        
    }
    have_a_try.changdu=n;
    have_a_try.quick_pai(have_a_try.box,0,n);
    have_a_try.printf_box();
    
}