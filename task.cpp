#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <queue>

using namespace std;

struct Task {
    int id;
    int duration;
    int priority;
    vector<int> dependencies;
};

void printLine(char ch, int length) {
    for (int i = 0; i < length; ++i) cout << ch;
    cout << endl;
}

void printTasks(const vector<Task>& tasks) {
    cout << left << setw(10) << "Task ID"
         << setw(12) << "Duration"
         << setw(10) << "Priority"
         << "Dependencies" << endl;
    printLine('-', 50);
    for (const auto& task : tasks) {
        cout << setw(10) << task.id
             << setw(12) << task.duration
             << setw(10) << task.priority;
        for (int dep : task.dependencies) {
            cout << dep << " ";
        }
        cout << endl;
    }
}

int knapsackSchedule(const vector<Task>& tasks, int maxTime, vector<Task>& selectedTasks) {
    int n = tasks.size();
    vector<vector<int>> dp(n + 1, vector<int>(maxTime + 1, 0));

    for (int i = 1; i <= n; ++i) {
        for (int t = 0; t <= maxTime; ++t) {
            if (tasks[i - 1].duration > t)
                dp[i][t] = dp[i - 1][t];
            else
                dp[i][t] = max(dp[i - 1][t], tasks[i - 1].priority + dp[i - 1][t - tasks[i - 1].duration]);
        }
    }

    int t = maxTime;
    for (int i = n; i > 0 && t >= 0; --i) {
        if (dp[i][t] != dp[i - 1][t]) {
            selectedTasks.push_back(tasks[i - 1]);
            t -= tasks[i - 1].duration;
        }
    }

    return dp[n][maxTime];
}

int greedySchedule(vector<Task> tasks, int maxTime, vector<Task>& selectedTasks) {
    sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        double r1 = (double)a.priority / a.duration;
        double r2 = (double)b.priority / b.duration;
        return r1 > r2;
    });

    int usedTime = 0, totalPriority = 0;
    for (const auto& task : tasks) {
        if (usedTime + task.duration <= maxTime) {
            selectedTasks.push_back(task);
            usedTime += task.duration;
            totalPriority += task.priority;
        }
    }
    return totalPriority;
}

vector<int> topologicalSort(const vector<Task>& tasks) {
    unordered_map<int, vector<int>> adj;  // dependency -> dependent tasks
    unordered_map<int, int> inDegree;

    // Initialize inDegree for all tasks
    for (const auto& task : tasks) {
        inDegree[task.id] = 0;
    }

    // Build graph edges and compute in-degree
    for (const auto& task : tasks) {
        for (int dep : task.dependencies) {
            adj[dep].push_back(task.id);
            inDegree[task.id]++;
        }
    }

    queue<int> q;
    for (const auto& [taskId, deg] : inDegree) {
        if (deg == 0) {
            q.push(taskId);
        }
    }

    vector<int> order;
    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        order.push_back(curr);

        for (int neighbor : adj[curr]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }

    if ((int)order.size() != (int)tasks.size()) {
        cout << "\n!!! Cycle detected: No valid task order!!!\n";
        return {};
    }

    return order;
}

void showDashboard() {
    printLine('=', 65);
    cout << setw(45) << "TASK SCHEDULING ALGORITHMS" << endl;
    printLine('=', 65);
    cout << left << setw(25) << "Option" << "Description\n";
    printLine('-', 65);
    cout << setw(25) << "1. 0/1 Knapsack DP" << "-> Maximize priority within time limit\n";
    cout << setw(25) << "2. Greedy" << "-> By priority/time ratio\n";
    cout << setw(25) << "3. Topological Sort" << "-> Order tasks by dependencies\n";
    cout << setw(25) << "4. Run All" << "-> Compare all algorithms\n";
    cout << setw(25) << "5. Exit" << "-> Close the program\n";
    printLine('=', 65);
}

vector<Task> getTasksInput(bool withDependencies = false) {
    int n;
    cout << "Enter the number of tasks: ";
    cin >> n;
    vector<Task> tasks(n);

    for (int i = 0; i < n; ++i) {
        cout << "\nTask " << (i + 1) << ":\n";
        tasks[i].id = i + 1;
        cout << "Duration: ";
        cin >> tasks[i].duration;
        cout << "Priority: ";
        cin >> tasks[i].priority;
        if (withDependencies) {
            cout << "Enter number of dependencies: ";
            int depCount;
            cin >> depCount;
            tasks[i].dependencies.resize(depCount);
            if (depCount > 0) {
                cout << "Enter dependency task IDs: ";
                for (int j = 0; j < depCount; ++j) {
                    cin >> tasks[i].dependencies[j];
                }
            }
        }
    }
    return tasks;
}

int main() {
    int choice;

    do {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        showDashboard();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                vector<Task> tasks = getTasksInput();
                int maxTime;
                cout << "Enter total available time: ";
                cin >> maxTime;

                vector<Task> knapsackTasks;
                int knapsackPriority = knapsackSchedule(tasks, maxTime, knapsackTasks);
                reverse(knapsackTasks.begin(), knapsackTasks.end());

                int usedTime = 0;
                for (const auto& task : knapsackTasks)
                    usedTime += task.duration;

                printLine('=', 50);
                cout << "0/1 Knapsack Result (Optimal)\n";
                printLine('=', 50);
                printTasks(knapsackTasks);
                cout << "\nTotal Priority Achieved: " << knapsackPriority << "\n";
                cout << "Time Used: " << usedTime << " / " << maxTime << " units\n";

                double efficiency = usedTime > 0 ? (double)knapsackPriority / usedTime : 0;
                cout << fixed << setprecision(2);
                cout << "Efficiency (Priority per Time Unit): " << efficiency << "\n";

                cout << "\nUnselected Tasks (Missed Opportunities):\n";
                printLine('-', 50);
                for (const auto& task : tasks) {
                    bool selected = false;
                    for (const auto& sel : knapsackTasks) {
                        if (task.id == sel.id) {
                            selected = true;
                            break;
                        }
                    }
                    if (!selected) {
                        cout << "Task ID: " << task.id
                             << ", Duration: " << task.duration
                             << ", Priority: " << task.priority << "\n";
                    }
                }

                system("pause");
                break;
            }

            case 2: {
                vector<Task> tasks = getTasksInput();
                int maxTime;
                cout << "Enter total available time: ";
                cin >> maxTime;

                vector<Task> greedyTasks;
                int greedyPriority = greedySchedule(tasks, maxTime, greedyTasks);

                printLine('=', 50);
                cout << "Greedy Result (Heuristic)\n";
                printLine('=', 50);
                printTasks(greedyTasks);
                cout << "Total Priority: " << greedyPriority << "\n\n";
                system("pause");
                break;
            }

            case 3: {
                vector<Task> tasks = getTasksInput(true);
                printLine('=', 50);
                cout << "Topological Sort (Dependency Order)\n";
                printLine('=', 50);
                vector<int> order = topologicalSort(tasks);
                if (!order.empty()) {
                    cout << "Valid Execution Order: ";
                    for (int id : order) cout << id << " ";
                    cout << "\n";
                }
                system("pause");
                break;
            }

            case 4: {
                vector<Task> tasks = getTasksInput(true);
                int maxTime;
                cout << "Enter total available time: ";
                cin >> maxTime;

                printLine('=', 50);
                cout << "Topological Sort (Dependency Order)\n";
                printLine('-', 50);
                vector<int> order = topologicalSort(tasks);
                if (!order.empty()) {
                    cout << "Execution Order: ";
                    for (int id : order) cout << id << " ";
                    cout << "\n";
                }

                vector<Task> knapsackTasks;
                int knapsackPriority = knapsackSchedule(tasks, maxTime, knapsackTasks);
                reverse(knapsackTasks.begin(), knapsackTasks.end());
                int usedTime = 0;
                for (const auto& task : knapsackTasks)
                    usedTime += task.duration;

                printLine('-', 50);
                cout << "0/1 Knapsack Result (Optimal)\n";
                printLine('-', 50);
                printTasks(knapsackTasks);
                cout << "Total Priority: " << knapsackPriority << "\n";
                cout << "Time Used: " << usedTime << " / " << maxTime << "\n";
                cout << "Efficiency: " << fixed << setprecision(2)
                     << (usedTime ? (double)knapsackPriority / usedTime : 0) << "\n";

                printLine('-', 50);
                cout << "Greedy Result (Heuristic)\n";
                vector<Task> greedyTasks;
                int greedyPriority = greedySchedule(tasks, maxTime, greedyTasks);
                printLine('-', 50);
                printTasks(greedyTasks);
                cout << "Total Priority: " << greedyPriority << "\n";
                system("pause");
                break;
            }

            case 5:
                cout << "Exiting program. Goodbye!\n";
                break;

            default:
                cout << "Invalid option. Please try again.\n";
                system("pause");
        }
    } while (choice != 5);

    return 0;
}
