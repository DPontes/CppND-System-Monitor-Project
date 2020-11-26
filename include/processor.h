#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long active_jiffies;
  long idle_jiffies;
  long prev_active_jiffies{0};
  long prev_idle_jiffies{0};
};

#endif
