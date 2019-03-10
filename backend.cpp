#include "backend.h"

Route::Route(const char *trainName, const char *srcName, const char *destName,
						 int startTime, int arriveTime, int fee):startTime(startTime),
arriveTime(arriveTime), fee(fee)
{
	strcpy(this->trainName, trainName);
	strcpy(this->srcName, srcName);
	strcpy(this->destName, destName);
	timeCost = arriveTime - startTime;
	next = NULL;
}

Route::Route()
{
	next = NULL;
}

SubResult::SubResult()
{

}

SubResult::SubResult(const char *trainName, const char *srcName,
										 const char *destName, int startTime, int arriveTime,
										 int fee):Route(trainName, srcName, destName, startTime,
																		arriveTime, fee)
{
	//显式调用父类有参构造函数要在初始化列表中
}

Result::Result():totalTimeCost(0), totalFee(0), totalStationNum(0)
{
	headSubResult = new SubResult();
	lastSubResult = headSubResult;
}

Result::~Result()
{
	SubResult *p = this->headSubResult;
	SubResult *np = NULL;
	while (p != NULL) {
		np = (SubResult *) p->next;
		delete p;
		p = np;
	}
}

void Result::addSubResult(const char *trainName, const char *srcName,
													const char *destName, int startTime, int arriveTime,
													int fee, enum Condition queryCondition)
{
	SubResult *tmpSubResult =
			new SubResult(trainName, srcName, destName, startTime, arriveTime, fee);
	int timeCost = 0;
	//TODO 跨日时间计算有问题
	if (startTime < lastSubResult->arriveTime && headSubResult != lastSubResult) {	//越过一天坐车
		timeCost = 24 + arriveTime - lastSubResult->startTime;
	} else {
		timeCost =
				arriveTime - (headSubResult !=
											lastSubResult ? lastSubResult->arriveTime : startTime);
	}
	lastSubResult->next = tmpSubResult;
	lastSubResult = tmpSubResult;
	totalFee += fee;
	totalStationNum++;
	totalTimeCost += timeCost;
	switch (queryCondition) {
	case LessFee:
		this->totalCost = totalFee;
		break;
	case LessTime:
		this->totalCost = totalTimeCost;
		break;
	case LessTransit:
		this->totalCost = totalStationNum;
		break;
	}
}

void Result::addSubResult(Route * route)
{
	this->addSubResult(route->trainName, route->srcName, route->destName,
										 route->startTime, route->arriveTime, route->fee,
										 route->queryCondition);
}

int Result::getTotalCost()
{
	return totalCost;
}

int Result::getTotalTimeCost()
{
	return totalTimeCost;
}

int Result::getTotalFee()
{
	return totalFee;
}

int Result::getTotalStationNum()
{
	return totalStationNum;
}

Backend::Backend():m_nNumCity(0)
{
	for (int i = 0; i < MAX_CITY_NUM; i++) {
		for (int j = 0; j < MAX_CITY_NUM; j++) {
			this->m_aMatrix[i][j] = new Route();	//建立头结点
			this->m_aRouteNum[i][j] = 0;
		}
	}
}

bool Backend::addCity(const char *cityName)
{
	int n = this->m_nNumCity;
	if (n > MAX_CITY_NUM)
		return false;
	strcpy(this->citys[n].cityName, cityName);
	this->citys[n].id = n;
	this->m_nNumCity++;
	return true;
}

bool Backend::removeCity(const char *cityName)
{
	int n = this->m_nNumCity;
	for (int i = 0; i < n; i++) {
		if (strcmp(cityName, this->citys[i].cityName) == 0) {
			this->m_nNumCity--;
			if (n > 1) {
				//用最后一个覆盖
				//删除与之相关的所有Route
				Route *p = NULL;
				for (int j = 0; j < n; j++) {
					//delete this->m_aMatrix[i][j];
					p = this->m_aMatrix[i][j]->next;	//有头结点跳过?
					while (p != NULL) {
						this->removeRoute(p->trainName);
						p = p->next;
					}
					this->m_aMatrix[i][j] = this->m_aMatrix[n - 1][j];	//覆盖
				}
				for (int j = 0; j < n; j++) {
					p = this->m_aMatrix[j][i]->next;
					while (p != NULL) {
						this->removeRoute(p->trainName);
						p = p->next;
					}
					this->m_aMatrix[j][i] = this->m_aMatrix[j][n - 1];	//覆盖
				}
				//删除City数组中对应内容
				strcpy(this->citys[i].cityName, this->citys[n - 1].cityName);
			}
			return true;
		}
	}
	return false;
}

Backend::~Backend()
{
	return;
}

bool Backend::addRoute(const char *trainName, const char *srcName,
											 const char *destName, int startTime, int arriveTime,
											 float feeCost)
{
	//不允许出现自环
	if (strcmp(srcName, destName) == 0)
		return false;
	//不允许同名列车出现
	//...
	int srcId = cityName2id(srcName);
	int destId = cityName2id(destName);
	if (srcId < 0 || destId < 0)	//未知城市
		return false;
	Route *newRoute =
			new Route(trainName, srcName, destName, startTime, arriveTime, feeCost);
	Route *p = this->m_aMatrix[srcId][destId];
	while (p->next != NULL)				//到达链表底部
		p = p->next;
	p->next = newRoute;
	this->m_aRouteNum[srcId][destId]++;
	return true;
}

bool Backend::modRoute(const char *trainName, const char *srcName,
											 const char *destName, int startTime, int arriveTime,
											 float feeCost)
{
	return this->removeRoute(trainName) &&
			this->addRoute(trainName, srcName, destName, startTime, arriveTime,
										 feeCost);
}

bool Backend::removeRoute(const char *trainName)
{
	int n = this->m_nNumCity;
	Route *p = NULL;
	Route *pre = NULL;
	int srcId, destId;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (this->m_aMatrix[i][j] != NULL) {
				pre = this->m_aMatrix[i][j];
				p = this->m_aMatrix[i][j]->next;
				while (p != NULL) {
					if (strcmp(p->trainName, trainName) == 0) {
						pre->next = p->next;
						srcId = cityName2id(p->srcName);
						destId = cityName2id(p->destName);
						this->m_aRouteNum[srcId][destId]--;
						delete p;
						return true;
					}
					pre = p;
					p = p->next;
				}
			}
		}
	}
	return false;
}

int Backend::cityName2id(const char *cityName)
{
	int n = this->m_nNumCity;
	for (int i = 0; i < n; i++) {
		if (strcmp(cityName, this->citys[i].cityName) == 0) {
			return i;
		}
	}
	return -1;										//没找到
}

Route *Backend::minCost(Route * headRoute, enum Condition queryCondition)
{
	if (headRoute == NULL)
		return NULL;
	headRoute = headRoute->next;	//第一个有效结点
	Route *minRoute = headRoute;
	Route *p = headRoute;
	if (headRoute == NULL)
		return NULL;
	int min = headRoute->cost;
	switch (queryCondition) {			//根据Condition找到最小值填充fake数组
	case LessFee:
		min = headRoute->fee;
		minRoute = headRoute;
		while (p) {
			if (p->fee < min) {
				min = p->fee;
				minRoute = p;
			}
			p = p->next;
		}
		minRoute->cost = min;
		return minRoute;
		break;
	case LessTime:
		min = headRoute->timeCost;
		minRoute = headRoute;
		while (p) {
			if (p->timeCost < min) {
				min = p->timeCost;
				minRoute = p;
			}
			p = p->next;
		}
		minRoute->cost = min;
		return minRoute;
		break;
	case LessTransit:
		headRoute->cost = 1;
		return headRoute;
		break;
	}
	return NULL;									// 无法连通
}

vector < vector < int >>Backend::dfs(const char *srcName, const char *destName)
{
	//函数调用前应填充对象的fake数组
	int vertexNum = m_nNumCity;
	int srcIndex = cityName2id(srcName), destIndex = cityName2id(destName);
	int s = srcIndex;
	int e = destIndex;
	bool vis[vertexNum];					//访问标志
	int pre[vertexNum];
	for (int i = 0; i < vertexNum; i++) {
		vis[i] = false;
		pre[i] = -1;
	}
	int v = s;
	vector < vector < int >>paths;
	vector < int >path;
	stack < int >sk;
	sk.push(v);
	vis[v] = true;
	//dfs
	int i = 0;
	do {
		for (; i < vertexNum; i++) {
			if (this->m_aFakeMatrix[v][i] != NULL && !vis[i]) {
				//邻接且没有被访问
				vis[i] = true;					//访问
				pre[i] = v;
				if (i == e) {
					//目标结点 记录路径
					int j = i;
					path.clear();
					path.push_back(e);
					while (pre[j] != -1) {
						path.push_back(pre[j]);
						j = pre[j];
					}
					vis[i] = false;				//最后一个结点在每条路径查找时应是未被访问的
					pre[i] = -1;
					reverse(path.begin(), path.end());
					paths.push_back(path);
					continue;
				}
				sk.push(i);							//记录i用于上一层恢复
				v = i;
				break;
			}
		}
		if (i == vertexNum) {				//s a
			i = sk.top();
			vis[i] = false;
			sk.pop();
			if (sk.empty())
				break;
			i++;
			v = sk.top();
		} else
			i = 0;
	} while (true);								//能找到下一结点
	return paths;
}

Result *Backend::getBestResult(const char *srcName, const char *destName,
															 enum Condition queryCondition)
{
	int src = cityName2id(srcName), dest = cityName2id(destName);
	int min = INT_MAX;
	Result *tmpRet = NULL;
	Result *ret = NULL;
	if (src >= 0 && dest >= 0) {
		//两城市存在
		for (int i = 0; i < m_nNumCity; i++) {
			for (int j = 0; j < m_nNumCity; j++) {
				//使用返回的trainName与cost字段来记录车次与权值
				this->m_aFakeMatrix[i][j] = minCost(this->m_aMatrix[i][j], queryCondition);	//最小权值Route填充fake数组 
			}
		}
		vector < vector < int >>paths = dfs(srcName, destName);
		if (paths.size() == 0)
			return NULL;
	for (auto path:paths) {
			tmpRet = new Result();
			for (int i = 0; i < (int)path.size() - 1; i++) {
				Route *route = this->m_aFakeMatrix[path[i]][path[i + 1]];
				route->queryCondition = queryCondition;
				tmpRet->addSubResult(route);
			}
			if (min >= tmpRet->getTotalCost()) {
				min = tmpRet->getTotalCost();
				if (ret != NULL)
					delete ret;
				ret = tmpRet;
			} else {
				delete tmpRet;
			}
		}
	}
	return ret;
}

bool cmp(Result * a, Result * b)
{
	return a->getTotalCost() < b->getTotalCost();
}

vector < Result * >Backend::getAllSortedResult(const char *srcName,
																							 const char *destName,
																							 enum Condition queryCondition)
{
	vector < Result * >ret;
	Result *route = NULL;
	int src = cityName2id(srcName), dest = cityName2id(destName);
	if (src >= 0 && dest >= 0) {
		//两城市存在
		for (int i = 0; i < m_nNumCity; i++) {
			for (int j = 0; j < m_nNumCity; j++) {
				//使用返回的trainName与cost字段来记录车次与权值
				this->m_aFakeMatrix[i][j] = minCost(this->m_aMatrix[i][j], queryCondition);	//最小权值Route填充fake数组 
			}
		}
		vector < vector < int >>paths = dfs(srcName, destName);
		if (paths.size() == 0)
			return ret;
		//排列组合生成完全列表
	for (auto path:paths) {
			int trainNum = path.size() - 1;
			Route *routeArr[trainNum];
			for (int i = 0; i < trainNum; i++) {
				routeArr[i] = this->m_aMatrix[path[i]][path[i + 1]]->next;
			}
			do {
				route = new Result;
				for (int i = 0; i < trainNum; i++) {
					routeArr[i]->queryCondition = queryCondition;
					route->addSubResult(routeArr[i]);
				}
				ret.push_back(route);
				routeArr[trainNum - 1] = routeArr[trainNum - 1]->next;
				for (int i = trainNum - 1; i > 0; i--) {
					if (routeArr[i] == NULL) {
						routeArr[i] = this->m_aMatrix[path[i]][path[i + 1]]->next;
						routeArr[i - 1] = routeArr[i - 1]->next;
					}
				}
				if (routeArr[0] == NULL)
					break;
			} while (true);
		}
		//排序ret
		sort(ret.begin(), ret.end(), cmp);
	}
	return ret;
}

//只要站点连通，那么这些站点的所有车都在All中，列出列表后按要求排序，最值Best也能求出
//Or
//All与Best分开，Best先排序用最少填充再dfs;All先dfs再排序 now
//可能都算不上求最短路径，只能算图的遍历
