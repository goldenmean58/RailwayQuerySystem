// 铁路系统后端
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stack>
#include <vector>
#include <algorithm>
using namespace std;
#define MAX_CITY_NUM 20
#define MAX_ROUTE_NUM 100
//查询条件
enum Condition { LessTime, LessFee, LessTransit };

typedef struct {
	char cityName[20];
	int id;
} City;

class Route {
public:
	char trainName[20];
	char srcName[20];
	char destName[20];
	int startTime;
	int arriveTime;
	int timeCost;
	int fee;
	int cost;											//随查询条件变化
	enum Condition queryCondition;
	Route *next;
	 Route(const char *trainName, const char *srcName, const char *destName,
				 int startTime, int arriveTime, int fee);
	 Route();
};

class SubResult:public Route {	//组车次带头结点链表
public:
	SubResult();
	SubResult(const char *trainName, const char *srcName, const char *destName,
						int startTime, int arriveTime, int fee);
};

class Result {
public:
	Result();
	~Result();
	void addSubResult(const char *trainName, const char *srcName,
										const char *destName, int startTime, int arriveTime,
										int fee, enum Condition queryCondition);
	void addSubResult(Route * route);
	int getTotalTimeCost();
	int getTotalFee();
	int getTotalStationNum();
	int getTotalCost();
	SubResult *headSubResult;
	SubResult *lastSubResult;
private:
	int totalTimeCost;
	int totalFee;
	int totalStationNum;					//总共多少站
	int totalCost;
};

class Backend {
public:
	Backend();
	//城市
	bool addCity(const char *cityName);	//添加城市信息
	bool removeCity(const char *cityName);
	//路线
	bool addRoute(const char *trainName, const char *srcName, const char *destName, int startTime, int arriveTime, float feeCost);	//添加添加路线
	bool modRoute(const char *trainName, const char *srcName, const char *destName, int startTime, int arriveTime, float feeCost);	//通过trainName修改路线
	bool removeRoute(const char *trainName);
	//查询方法
	Result *getBestResult(const char *srcName, const char *destName, enum Condition queryCondition);	//查询最佳路线(调用getAllSortedResult来取得)
	 vector < Result * >getAllSortedResult(const char *srcName, const char *destName, enum Condition queryCondition);	//查询所有方案并按给定条件进行排序结果
	 virtual ~ Backend();
	City citys[MAX_CITY_NUM];
	int m_nNumCity;								//当前城市数量
	Route *m_aMatrix[MAX_CITY_NUM][MAX_CITY_NUM];	//三种花费的邻接矩阵内容为链表来实现多趟列车
private:
	int cityName2id(const char *cityName);	//城市名转citys数组index同时也对应邻接矩阵
	//Route *getMinCostRoute(int i, int j, enum Condition queryCondition);
	Route *minCost(Route * headRoute, enum Condition queryCondition);
	Route *m_aFakeMatrix[MAX_CITY_NUM][MAX_CITY_NUM];	//对应于m_aMatrix条件对应转化的临时数组用到其中cost与trainName
	int m_aRouteNum[MAX_CITY_NUM][MAX_CITY_NUM];	//两站之间的列车总数
	 vector < vector < int >>dfs(const char *srcName, const char *destName);
};
