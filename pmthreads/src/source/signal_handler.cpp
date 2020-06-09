/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include "signal_handler.hpp"
#include <atomic>
#include <assert.h>
#include <pthread.h>


pthread_t sig_thread;
std::atomic<bool> _alarmed;

static void * sig_threadproc(void *thrarg)
{
    (void)(thrarg);
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    
    //pid_t x = syscall(__NR_gettid);
    
    //barrier_cross(&barrier);
    //while(!__stop__.load(std::memory_order_relaxed))
    for(;;){
        int sig;
        int error;
        
        error = sigwait(&sigset, &sig);
        if (error == 0) {
            assert(sig == SIGALRM);
            //fprintf(stderr, "alarmed\n");
            _alarmed.store(true, std::memory_order_release);
        } else {
            perror("sigwait");
        }
    }
    return NULL;
}

//call during initialization
void init_sig_alarm_handler(){
    //BLOCK SIGALRM on main thread
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    int s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    assert(s==0);
    (void)(s);
    _alarmed.store(false, std::memory_order_release);
    
    pthread_create(&sig_thread, NULL, sig_threadproc, NULL);
}

//call during finilization
void finalize_sig_alarm_handler(){
    pthread_cancel(sig_thread);
}
