// headers
#include <stdio.h>
#include <stdlib.h>

#include "scene.h"

#define QUEUE_SIZE 25

typedef struct _queue {

    int front;
    int rear;
    int count;
    Scene *data[QUEUE_SIZE];

} Queue;

Queue *SceneQueue = NULL;

bool isEmpty()
{
    return (!SceneQueue || SceneQueue->front == -1 || SceneQueue->front > SceneQueue->rear);
}

bool InitSceneQueue()
{
	// create queue
	SceneQueue = (Queue*)malloc(sizeof(Queue));

    // fail if malloc failed
    if (!SceneQueue) return false;

	SceneQueue->front = -1;
	SceneQueue->rear = -1;
	SceneQueue->count = 0;

	return true;
}

bool AddScene(Scene* scene)
{
    // error conditions
    if (SceneQueue == NULL) return false;

    // if it is the first element, set front and rear both
    if (SceneQueue->front == -1 && SceneQueue->rear == -1)
    {
        SceneQueue->front = 0;
        SceneQueue->rear = 0;
    }
    else
    {
        SceneQueue->rear++;
    }

    SceneQueue->data[SceneQueue->rear] = scene;
    SceneQueue->count++;
    return true;
}

bool RemoveScene()
{
    // error conditions
    if (SceneQueue == NULL || isEmpty()) return false;

    // soft delete the scene
    // SceneQueue->data[SceneQueue->front] = NULL;
    // SceneQueue->count--;
    SceneQueue->front++;
    return true;
}

bool GetScene(Scene& scene)
{
    // error conditions
    if (isEmpty()) return false;

    scene = *SceneQueue->data[SceneQueue->front];
    return true;
}

bool GetSceneAt(Scene& scene, int idx)
{
    // error conditions
    if (isEmpty()) return false;
    if (idx >= SceneQueue->count) return false;

    scene = *SceneQueue->data[idx];
    return true;
}

int GetSceneCount()
{
    // error conditions
    if (isEmpty()) return 0;

    return SceneQueue->count;
}

