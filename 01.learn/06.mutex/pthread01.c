#include <stdio.h>
#include <pthread.h>

/**
 * 需要生产的数量
 */
// #define MAX 1000000000
#define MAX 100

pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;

// 生产者消费者使用的缓冲区
int buffer = 0;

/**
 * 生产数据
 * @brief producer
 * 
 * @param ptr 
 * @return void* 
 */
void *producer(void *ptr) {
  int i;
  for (i = 1; i <= MAX; i++) {
    // 互斥使用缓冲区
    pthread_mutex_lock(&the_mutex);
    while (buffer != 0) {
      pthread_cond_wait(&condp, &the_mutex);
    }
    buffer = i;
    printf("producer produce data [%d] \n", buffer);
    pthread_cond_signal(&condc);
    pthread_mutex_unlock(&the_mutex);

  }
  pthread_exit(0);
}

/**
 * 消费数据
 * @brief 消费者
 * 
 * @param ptr 
 * @return void* 
 */
void *consumer(void *ptr) {
  int i;
  for (i = 1; i <= MAX; i++) {
    pthread_mutex_lock(&the_mutex);
    while (buffer == 0) {
      pthread_cond_wait(&condc, &the_mutex);
    }
    printf("consumer consume data [%d] \n", buffer);
    buffer = 0;
    pthread_cond_signal(&condp);
    pthread_mutex_unlock(&the_mutex);
    
  }
  pthread_exit(0);
}

int main(int argc, char **argv) {
  pthread_t pro, con;
  pthread_mutex_init(&the_mutex, 0);
  pthread_cond_init(&condc, 0);
  pthread_cond_init(&condp, 0);
  pthread_create(&con, 0, consumer, 0);
  pthread_create(&pro, 0, producer, 0);
  pthread_join(pro, 0);
  pthread_join(con, 0);
  pthread_cond_destroy(&condc);
  pthread_cond_destroy(&condp);
  pthread_mutex_destroy(&the_mutex);

}