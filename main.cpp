#include "backend.h"

void outputResult(Result* ret){
    Route *subResult;
    if(ret==NULL){
        cout<<"两地无法到达!"<<endl;
    }else{
        subResult=ret->headSubResult->next;
        cout<<subResult->srcName<<" - ";
        while(subResult){
            cout<<subResult->trainName<<" - "<<subResult->destName;
            subResult=subResult->next;
            if(subResult) cout<< " - ";
        }
        cout<<endl;
        cout<<"Total Time:"<<ret->getTotalTimeCost()
            <<'\t'<<"Total Fee:"<<ret->getTotalFee()
            <<'\t'<<"Total Transit:"<<ret->getTotalStationNum() - 1
            <<endl;
    }
    delete ret;
}

int main(void){
    Backend backend; //建立后端
    int choice;
    char strBuff[255];
    char trainName[20];
    char srcName[20];
    char destName[20];
    int startTime;
    int arriveTime;
    int fee;
    int condition;
    Route *p;
    Result *ret;
    vector<Result *>rets;
    while(true){
        ret=NULL;
        cout<<"欢迎使用全国铁路交通咨询系统"<<endl;
        cout<<"功能区:"<<endl<<endl;
        cout<<" 1. 添加城市"<<endl;
        cout<<" 2. 删除城市"<<endl;
        cout<<" 3. 列出城市"<<endl;
        cout<<" 4. 增开列车"<<endl;
        cout<<" 5. 调动列车"<<endl;
        cout<<" 6. 删除车次"<<endl;
        cout<<" 7. 列出车次"<<endl;
        cout<<" 8. 最优查询"<<endl;
        cout<<" 9. 总览查询"<<endl;
        cout<<"10. 退出程序"<<endl<<endl;
        cout<<"请选择:";
        cin>>choice;
        switch(choice){
            case 1:
                cout<<"请输入要添加的城市名:";
                cin>>strBuff;
                if(backend.addCity(strBuff)){
                    cout<<"添加城市成功!"<<endl;
                }else{
                    cout<<"城市已存在,添加失败!"<<endl;
                }
                break;
            case 2:
                cout<<"请输入要删除的城市名:";
                cin>>strBuff;
                if(backend.removeCity(strBuff)){
                    cout<<"删除城市成功!"<<endl;
                }else{
                    cout<<"城市不存在,删除城市失败!"<<endl;
                }
                break;
            case 3:
                for(int i=0;i<backend.m_nNumCity;i++){
                    cout<<backend.citys[i].cityName<<endl;
                }
                break;
            case 4:
                cout<<"请输入列车名:";
                cin>>trainName;
                cout<<"请输入起点站名:";
                cin>>srcName;
                cout<<"请输入终点站名:";
                cin>>destName;
                cout<<"请输入发车时间:";
                cin>>startTime;
                cout<<"请输入到达时间:";
                cin>>arriveTime;
                cout<<"请输入车票费用:";
                cin>>fee;
                if(backend.addRoute(trainName,srcName,destName,startTime,arriveTime,fee)){
                    cout<<"添加车次成功!"<<endl;
                }else{
                    cout<<"添加车次失败!"<<endl;
                }
                break;
            case 5:
                cout<<"请输入要调动的列车名:";
                cin>>trainName;
                cout<<"请输入起点站名:";
                cin>>srcName;
                cout<<"请输入终点站名:";
                cin>>destName;
                cout<<"请输入发车时间:";
                cin>>startTime;
                cout<<"请输入到达时间:";
                cin>>arriveTime;
                cout<<"请输入车票费用:";
                cin>>fee;
                if(backend.modRoute(trainName,srcName,destName,startTime,arriveTime,fee)){
                    cout<<"修改车次成功!"<<endl;
                }else{
                    cout<<"修改车次失败!"<<endl;
                }
                break;
            case 6:
                cout<<"请输入要删除的列车名:";
                cin>>trainName;
                if(backend.removeRoute(trainName)){
                    cout<<"删除车次成功!"<<endl;
                }else{
                    cout<<"删除车次失败!"<<endl;
                }
                break;
            case 7:
                cout<<setw(15)<<"Train Name"<<setw(15)<<"Start Station"<<setw(15)<<"End Station"<<setw(15)<<"Start Time"<<setw(15)<<"Arrive Time"<<setw(15)<<"Ticket Fee"<<endl;
                for(int i=0;i<backend.m_nNumCity;i++){
                    for(int j=0;j<backend.m_nNumCity;j++){
                        p=backend.m_aMatrix[i][j]->next;
                        while(p!=NULL){
                            cout<<setw(15)<<p->trainName<<setw(15)<<p->srcName<<setw(15)<<p->destName<<setw(15)<<p->startTime<<setw(15)<<p->arriveTime<<setw(15)<<p->fee<<endl;
                            p=p->next;
                        }
                    }
                }
                break;
            case 8:
                cout<<"请输入起始地:";
                cin>>srcName;
                cout<<"请输入目的地:";
                cin>>destName;
                cout<<"请选择决策(1.最短时间 2.最少旅费 3.中转最少 4.所有最佳方案):";
                cin>>condition;
                switch(condition){
                    case 1:
                        ret=backend.getBestResult(srcName,destName,LessTime);
                        outputResult(ret);
                        break;
                    case 2:
                        ret=backend.getBestResult(srcName,destName,LessFee);
                        outputResult(ret);
                        break;
                    case 3:
                        ret=backend.getBestResult(srcName,destName,LessTransit);
                        outputResult(ret);
                        break;
                    default:
                        ret=backend.getBestResult(srcName,destName,LessTime);
                        outputResult(ret);
                        ret=backend.getBestResult(srcName,destName,LessFee);
                        outputResult(ret);
                        ret=backend.getBestResult(srcName,destName,LessTransit);
                        outputResult(ret);
                        break;
                }
                break;
            case 9:
                cout<<"请输入起始地:";
                cin>>srcName;
                cout<<"请输入目的地:";
                cin>>destName;
                cout<<"请选择决策(1.最短时间 2.最少旅费 3.中转最少):";
                cin>>condition;
                switch(condition){
                    case 1:
                        rets=backend.getAllSortedResult(srcName,destName,LessTime);
                        break;
                    case 2:
                        rets=backend.getAllSortedResult(srcName,destName,LessFee);
                        break;
                    case 3:
                        rets=backend.getAllSortedResult(srcName,destName,LessTransit);
                        break;
                }
                if(rets.size()>0){
                    for(auto iter : rets){
                        outputResult(iter);
                    }
                }else{
                    cout<<"无路径到达!"<<endl;
                }
                break;
            case 10:
                return 0;
            default:
                break;
        }
        cout<<endl<<endl;
    }
    return 0;
}
