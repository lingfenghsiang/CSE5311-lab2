#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

// the structure to hold the graph, it needs to be aboe be update the graph
class Edge
{
public:
    int head, capacity;
    Edge();
};
class NodeInfo : public Edge
{
public:
    vector<Edge> connectedEdges;
    int connectedEdgeNum;
    NodeInfo();
};
class Graph : public NodeInfo
{
public:
    vector<NodeInfo> contents;
    int nodeNum, maxFlow;
    Graph(int nodeSize);
    void addItem(int tail, int head, int capacity);
    int partition(int nodeIndex, int left, int right);
    void quickSortEdge(int nodeIndex, int left, int right);
    // merge the items of the same edge
    void mergeEdge(int nodeIndex);
    void updateNodes();
    // find the corresponding edge and its capacity
    int nodeIndexBinSearch(int tail, int head);
    void findMaxFlow(int source, int sink);
};

float CPUtime();

const int exponentialSeuqnce[] = {
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000004,
    0x00000008,
    0x00000010,
    0x00000020,
    0x00000040,
    0x00000080,
    0x00000100,
    0x00000200,
    0x00000400,
    0x00000800,
    0x00001000,
    0x00002000,
};

int main(void)
{
    int head, tail, capacity;
    int vertexNum, edgeNum;
    float startCPU, stopCPU;
    cout << "Please input the number of vertices and edges first, then information about the edges" << endl;
    cin >> vertexNum >> edgeNum;
    Graph inputData(vertexNum);
    for (int i = 0; i < edgeNum; i++)
    {
        cin >> head >> tail >> capacity;
        inputData.addItem(head, tail, capacity);
    }
    startCPU = CPUtime();
    inputData.updateNodes();
    stopCPU = CPUtime();
    cout << "time for sorting and merging input data is " << stopCPU - startCPU << endl;
    startCPU = CPUtime();
    inputData.findMaxFlow(0, 499);
    stopCPU = CPUtime();
    cout << "Ford-Fulkerson(Capacity scaling) time is " << stopCPU - startCPU << endl;
    return 0;
}

Edge::Edge()
{
    head = 0;
    capacity = 0;
}
NodeInfo::NodeInfo()
{
    connectedEdgeNum = 0;
    connectedEdges.resize(64);
}
Graph::Graph(int nodeSize)
{
    contents.resize(nodeSize);
    nodeNum = nodeSize;
}
void Graph::addItem(int tail, int head, int capacity)
{
    if (tail != head)
    {
        int counter = 0, vectorSize = 0;
        // add corresponding items
        contents[tail].connectedEdgeNum += 1;
        counter = contents[tail].connectedEdgeNum - 1;
        vectorSize = contents[tail].connectedEdges.size();
        if (counter >= vectorSize - 1)
            contents[tail].connectedEdges.resize(vectorSize * 2);
        contents[tail].connectedEdges[counter].head = head;
        contents[tail].connectedEdges[counter].capacity = capacity;
        // add the capacity in the reverse direction
        contents[head].connectedEdgeNum += 1;
        counter = contents[head].connectedEdgeNum - 1;
        vectorSize = contents[head].connectedEdges.size();
        if (counter >= vectorSize - 1)
            contents[head].connectedEdges.resize(vectorSize * 2);
        contents[head].connectedEdges[counter].head = tail;
        contents[head].connectedEdges[counter].capacity = 0;
    }
}
// devide the sequence, to be used in quick sort
int Graph::partition(int nodeIndex, int left, int right)
{
    Edge pivotValue = contents[nodeIndex].connectedEdges[right];
    Edge tmpExchange;
    int storeIndex = left;
    for (int i = left; i < right; i++)
    {
        if (contents[nodeIndex].connectedEdges[i].head <= pivotValue.head)
        {
            tmpExchange = contents[nodeIndex].connectedEdges[i];
            contents[nodeIndex].connectedEdges[i] = contents[nodeIndex].connectedEdges[storeIndex];
            contents[nodeIndex].connectedEdges[storeIndex] = tmpExchange;
            storeIndex += 1;
        }
    }
    tmpExchange = contents[nodeIndex].connectedEdges[right];
    contents[nodeIndex].connectedEdges[right] = contents[nodeIndex].connectedEdges[storeIndex];
    contents[nodeIndex].connectedEdges[storeIndex] = tmpExchange;
    return storeIndex;
}
// sort the sequence of the connected nodes
void Graph::quickSortEdge(int nodeIndex, int left, int right)
{
    int pivot;
    if (left < right)
    {
        pivot = partition(nodeIndex, left, right);
        quickSortEdge(nodeIndex, left, pivot - 1);
        quickSortEdge(nodeIndex, pivot + 1, right);
    }
}
// merge duplicate edge
void Graph::mergeEdge(int nodeIndex)
{
    if (contents[nodeIndex].connectedEdgeNum >= 1)
    {
        quickSortEdge(nodeIndex, 0, contents[nodeIndex].connectedEdgeNum - 1);
        int newEdgeNum = 1;
        int originalEdgeNum = contents[nodeIndex].connectedEdgeNum;
        int presentEdgeIndex = 0;
        int presentHead = contents[nodeIndex].connectedEdges[0].head;
        for (int i = 1; i < originalEdgeNum; i++)
        {
            if (contents[nodeIndex].connectedEdges[i].head == presentHead)
            {
                contents[nodeIndex].connectedEdges[presentEdgeIndex].capacity +=
                    contents[nodeIndex].connectedEdges[i].capacity;
            }
            else
            {
                newEdgeNum += 1;
                presentEdgeIndex += 1;
                presentHead = contents[nodeIndex].connectedEdges[i].head;
                contents[nodeIndex].connectedEdges[presentEdgeIndex].capacity =
                    contents[nodeIndex].connectedEdges[i].capacity;
                contents[nodeIndex].connectedEdges[presentEdgeIndex].head =
                    contents[nodeIndex].connectedEdges[i].head;
            }
        }
        contents[nodeIndex].connectedEdgeNum = newEdgeNum;
        contents[nodeIndex].connectedEdges.resize(newEdgeNum);
    }
}
void Graph::updateNodes()
{
    for (int i = 0; i < nodeNum; i++)
        mergeEdge(i);
}
int Graph::nodeIndexBinSearch(int tail, int head)
{
    int right = contents[tail].connectedEdgeNum - 1, left = 0, middle;
    if (right < left)
    {
        cout << "input error" << endl;
        exit(0);
    }
    for (;;)
    {
        middle = left + (right - left) / 2;
        if (head > contents[tail].connectedEdges[middle].head)
            left = middle;
        else if (head < contents[tail].connectedEdges[middle].head)
            right = middle;
        if (head == contents[tail].connectedEdges[middle].head)
            return middle;
        else if (right - left <= 1 && head == contents[tail].connectedEdges[left].head)
            return left;
        else if (right - left <= 1 && head == contents[tail].connectedEdges[right].head)
            return right;
        else if (right - left <= 1)
        {
            cout << "there is no such edge" << endl;
            exit(0);
        }
    }
}
void Graph::findMaxFlow(int source, int sink)
{
    // find the largest capacity edge of the source node
    int maxSrcEdgeCapacity = 0, delta = 0, findFlag = 0, augmentingPathNum = 0;
    float timer1=0, timer1Start, timer1End, timer2=0, timer2Start, timer2End, timer3=0;
    maxFlow = 0;
    queue<int> nodesToVisit;
    vector<int> flowTraceBack(nodeNum, -1);
    vector<int> visitedSet(nodeNum, 0);
    int presentNode;
    for (int i = 0; i < contents[source].connectedEdgeNum; i++)
    {
        if (maxSrcEdgeCapacity < contents[source].connectedEdges[i].capacity)
            maxSrcEdgeCapacity = contents[source].connectedEdges[i].capacity;
    }
    cout << "max src edge capacity is " << maxSrcEdgeCapacity << endl;
    for (int i = 0; i < 15; i++)
    {
        if (maxSrcEdgeCapacity > exponentialSeuqnce[i])
            delta = exponentialSeuqnce[i];
        else
            break;
    }
    cout << "delta is " << delta << endl;
    // breadth first search
    int currentPlace;

    for (;;)
    {
        nodesToVisit.push(source);
        cout << "scaling parameter is " << delta << endl;
        for (;;)
        {
            currentPlace = nodesToVisit.front();
            if (currentPlace > 500)
                cout << "problem" << endl;
            // if not the end, push into the queue
            if (currentPlace != sink)
            {
                timer1Start = CPUtime();
                for (int i = 0; i < contents[currentPlace].connectedEdgeNum; i++)
                {
                    if (visitedSet[contents[currentPlace].connectedEdges[i].head] == 0 &&
                        contents[currentPlace].connectedEdges[i].capacity >= delta)
                    {
                 
                        flowTraceBack[contents[currentPlace].connectedEdges[i].head] = currentPlace;
                        nodesToVisit.push(contents[currentPlace].connectedEdges[i].head);
                   
                    }
                }
                timer1End = CPUtime();
                timer1 += (timer1End - timer1Start);
            }
            else if (currentPlace == sink)
            {
                timer2Start = CPUtime();
                // an augmenting path is found, traceback
                int trace = currentPlace;
                augmentingPathNum += 1;
                int minCapacityOnPath = 0x0fffffff, preNode, postNode, headPosition;
                cout << sink;
                for (;;)
                {
                    postNode = trace;
                    trace = flowTraceBack[trace];
                    preNode = trace;
                    if (preNode < 0)
                        break;
                    headPosition = nodeIndexBinSearch(preNode, postNode);
                    if (minCapacityOnPath > contents[preNode].connectedEdges[headPosition].capacity)
                        minCapacityOnPath = contents[preNode].connectedEdges[headPosition].capacity;
                    cout << "<-" << trace;
                }
                cout << " adds " << minCapacityOnPath << " incremental flow" << endl;
                // renew residual capacity
                trace = currentPlace;
                for (;;)
                {
                    postNode = trace;
                    trace = flowTraceBack[trace];
                    preNode = trace;
                    if (preNode < 0)
                        break;
                    headPosition = nodeIndexBinSearch(preNode, postNode);
                    contents[preNode].connectedEdges[headPosition].capacity -= minCapacityOnPath;
                    headPosition = nodeIndexBinSearch(postNode, preNode);
                    contents[postNode].connectedEdges[headPosition].capacity += minCapacityOnPath;
                }
                visitedSet.assign(nodeNum, 0);
                flowTraceBack.assign(flowTraceBack.size(), -1);
                while (!nodesToVisit.empty())
                    nodesToVisit.pop();
                nodesToVisit.push(source);
                findFlag = 1;
                maxFlow += minCapacityOnPath;
                timer2End = CPUtime();
                timer2 += (timer2End - timer2Start);
            }
            // find new augmenting path
            if (findFlag == 1)
            {
                findFlag = 0;
            }
            else
            {
                visitedSet[currentPlace] = 1;
                nodesToVisit.pop();
            }

            if (nodesToVisit.empty())
                break;
        }
        visitedSet.assign(nodeNum, 0);
        if (delta == 1)
            break;
        delta = delta >> 1;
    }
    cout << augmentingPathNum << " augmenting paths" << endl;
    cout << "the maximum flow is " << maxFlow << endl;
    cout << "To clarify,";
    cout << "the queue time is " << timer1 << ", ";
    cout << "the rest of algorithm time is " << timer2 << endl;
}
float CPUtime()
{
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
    return rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec / 1000000.0 + rusage.ru_stime.tv_sec + rusage.ru_stime.tv_usec / 1000000.0;
}

