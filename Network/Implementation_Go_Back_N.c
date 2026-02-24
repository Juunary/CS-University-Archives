#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#pragma warning(disable:4996)

#define LOSS_PROB 0.2       // Probability that a packet is lost in the network
#define CORRUPT_PROB 0.1    // Probability that a packet is corrupted in the network
#define SIM_MESSAGE_MAX 50  // Number of messages to simulate from layer 5

#define BIDIRECTIONAL 1     // Enable bi-directional communication
#define TIME_AVERAGE 5      // Average time interval between messages
#define TIMERINCREMENT 30.0 // Time interval for retransmission timer

#define TIMER_INTERRUPT 0  
#define FROM_LAYER5 1
#define FROM_LAYER3 2

#define OFF 0
#define ON 1
#define A 0
#define B 1

#define mmm RAND_MAX

struct msg {                // Message from layer 5 (application)
    char data[20];
};

struct pkt {                // Network layer packet with basic header and payload
    int seqnum;
    int acknum;
    int checksum;
    char payload[20];
};

struct event {              // Event in the event-driven simulator
    float evtime;
    int evtype;
    int eventity;
    struct pkt packet;
    struct event* prev;
    struct event* next;
};

struct event* evlist = NULL;
int TRACE = 3;
int nsim = 0;
int nsimmax = 0;
float time = 0.000;
float lossprob;
float corruptprob;
float lambda;
int ntolayer3 = 0;          // Total packets sent to layer 3
int nlost = 0;              // Total lost packets
int ncorrupt = 0;           // Total corrupted packets

int bytes_delivered_to_layer5 = 0;  // Number of bytes delivered to application

void tolayer3(int AorB, struct pkt packet);
void tolayer5(int AorB, char datasent[20]);
void starttimer(int AorB, float increment);
void stoptimer(int AorB);
void generate_next_arrival(void);
void insertevent(struct event* p);
float jimsrand(void);
void init(void);
void printevlist(void);


/********* Protocol-side functions (to be implemented by student) **************/

#define N 8

/* Sender A variables */
int A_base = 0;
int A_nextseqnum = 0;
struct pkt A_window[N];
int A_lastACKToSend = -1;
int A_expectedseqnum;

/* Receiver B variables */
int B_base = 0;
int B_nextseqnum = 0;
struct pkt B_window[N];
int B_lastACKToSend = -1;
int B_expectedseqnum;

/**
 * compute_checksum - Calculate checksum for packet integrity verification
 * @packet: The packet for which checksum is computed
 * 
 * Returns the sum of sequence number, acknowledgment number, and payload bytes
 */
int compute_checksum(struct pkt packet) {
    int sum = packet.seqnum + packet.acknum;
    for (int i = 0; i < 20; i++) sum += packet.payload[i];
    return sum;
}

/**
 * A_output - Process outgoing message from layer 5 at sender A
 * @message: Message data from application layer
 * 
 * Implements Go-Back-N transmission: sends packet if window space is available,
 * piggybacks ACK if available, and starts timer for first packet
 */
void A_output(struct msg message) {
    // Check if there is available window space
    if (A_nextseqnum < A_base + N) {
        struct pkt p;
        p.seqnum   = A_nextseqnum;
        // Piggyback acknowledgment if available; otherwise set to 999
        p.acknum   = (A_lastACKToSend >= 0) ? A_lastACKToSend : 999;
        memcpy(p.payload, message.data, sizeof(p.payload));
        p.checksum = compute_checksum(p);
        A_window[A_nextseqnum % N] = p;
        tolayer3(A, p);
        // Start timer only for the first packet in window
        if (A_base == A_nextseqnum)
            starttimer(A, TIMERINCREMENT);
        A_nextseqnum++;
        A_lastACKToSend = -1;  // Reset after piggybacking
    }
}

/**
 * A_input - Process incoming packet from layer 3 at receiver A
 * @packet: Received packet from network layer
 * 
 * Validates checksum, handles acknowledgments (cumulative), and processes
 * out-of-order/duplicate data packets
 */
void A_input(struct pkt packet) {
    // Verify packet integrity
    if (compute_checksum(packet) != packet.checksum) return;

    /* Check if this is an acknowledgment packet (from B to A) */
    if (packet.acknum >= A_base && packet.acknum < A_nextseqnum) {    
        /* Process cumulative acknowledgment at sender side */ 
        A_base = packet.acknum + 1;
        if (A_base == A_nextseqnum)
            stoptimer(A);
        else {
            stoptimer(A);
            starttimer(A, TIMERINCREMENT);
        }
        return;
    }

    /* Check if this is a data packet (B → A direction) */
    if (packet.seqnum == A_expectedseqnum) {
        tolayer5(A, packet.payload);
        A_lastACKToSend = A_expectedseqnum;
        A_expectedseqnum++;

    } else {
        /* Retransmit last acknowledgment for duplicate/out-of-order packets */
        int lastack = A_expectedseqnum - 1;
        if (lastack >= 0) {
            struct pkt ackpkt = {0, lastack, 0, {0}};
            ackpkt.checksum = compute_checksum(ackpkt);
            tolayer3(A, ackpkt);
        }
    }
}           

/**
 * A_timerinterrupt - Handle timeout event at sender A
 * 
 * Retransmits all packets in current window when timeout occurs,
 * implements standard Go-Back-N timeout mechanism
 */
void A_timerinterrupt() {
    /* Retransmit all packets in window on timeout */
    starttimer(A, TIMERINCREMENT);
    for (int i = A_base; i < A_nextseqnum; i++)
        tolayer3(A, A_window[i % N]);
}

/**
 * A_init - Initialize sender A state variables
 * 
 * Resets all sender and receiver state for entity A
 */
void A_init() {
    A_base            = 0;
    A_nextseqnum      = 0;
    A_expectedseqnum  = 0;
    A_lastACKToSend   = -1;    
}

/**
 * B_output - Process outgoing message from layer 5 at sender B
 * @message: Message data from application layer
 * 
 * Implements Go-Back-N transmission (identical logic to A_output)
 */
void B_output(struct msg message) {
    // Check if there is available window space
    if (B_nextseqnum < B_base + N) {
        struct pkt p;
        p.seqnum   = B_nextseqnum;
        p.acknum   = 0;
        memcpy(p.payload, message.data, sizeof(p.payload));
        p.checksum = compute_checksum(p);
        B_window[B_nextseqnum % N] = p;
        tolayer3(B, p);
        // Start timer only for the first packet in window
        if (B_base == B_nextseqnum)
            starttimer(B, TIMERINCREMENT);
        B_nextseqnum++;
    }
}

/**
 * B_input - Process incoming packet from layer 3 at receiver B
 * @packet: Received packet from network layer
 * 
 * Validates checksum, handles acknowledgments, and processes
 * out-of-order/duplicate data packets
 */
void B_input(struct pkt packet) {
    /* Verify packet integrity using checksum */
    if (compute_checksum(packet) != packet.checksum) return;

    /* Check if this is an acknowledgment packet (from A to B) */
    if (packet.acknum >= B_base && packet.acknum < B_nextseqnum) {
        /* Process cumulative acknowledgment at sender side */
        B_base = packet.acknum + 1;
        if (B_base == B_nextseqnum)
            stoptimer(B);
        else {
            stoptimer(B);
            starttimer(B, TIMERINCREMENT);
        }
        return;
    }

    /* Check if this is a data packet (A → B direction) */
    if (packet.seqnum == B_expectedseqnum) {
        tolayer5(B, packet.payload);
        B_lastACKToSend = B_expectedseqnum;
        B_expectedseqnum++;
    } else {
        /* Retransmit last acknowledgment for duplicate/out-of-order packets */
        int lastack = B_expectedseqnum - 1;
        if (lastack >= 0) {
            struct pkt ackpkt = {0, lastack, 0, {0}};
            ackpkt.checksum = compute_checksum(ackpkt);
            tolayer3(B, ackpkt);
        }
    }
}

/**
 * B_timerinterrupt - Handle timeout event at sender B
 * 
 * Retransmits all packets in current window [B_base, B_nextseqnum) when timeout occurs
 */
void B_timerinterrupt() {
    /* Retransmit all packets in window on timeout */
    starttimer(B, TIMERINCREMENT);
    for (int i = B_base; i < B_nextseqnum; i++) {
        tolayer3(B, B_window[i % N]);
    }
}

/**
 * B_init - Initialize receiver B state variables
 * 
 * Resets all sender and receiver state for entity B
 */
void B_init() {
    B_base            = 0;
    B_nextseqnum      = 0;
    B_expectedseqnum  = 0;
    B_lastACKToSend   = -1;    
}

/********************* SIMULATOR CODE *********************/

/**
 * main - Event-driven network simulator main loop
 * 
 * Initializes entities A and B, then processes events from the event queue
 * until simulation completes. Reports throughput statistics.
 */
int main() {
    struct event* eventptr;
    struct msg msg2give;
    struct pkt pkt2give;
    int i, j;

    init();         // Initialize simulation environment
    A_init();       // Initialize sender A
    B_init();       // Initialize sender B

    while (1) {
        eventptr = evlist;          // Fetch next event from event list
        if (eventptr == NULL) break;
        evlist = evlist->next;
        if (evlist != NULL) evlist->prev = NULL;
        if (TRACE >= 2) {
            printf("\nEVENT time: %f,", eventptr->evtime);
            printf("  type: %d", eventptr->evtype);
            if (eventptr->evtype == 0)
                printf(", timerinterrupt  ");
            else if (eventptr->evtype == 1)
                printf(", fromlayer5 ");
            else
                printf(", fromlayer3 ");
            printf(" entity: %d\n", eventptr->eventity);
        }
        time = eventptr->evtime;    // Advance simulation clock

        if (nsim == nsimmax) break;
        
        /* Handle different event types */
        if (eventptr->evtype == FROM_LAYER5) {
            generate_next_arrival();    // Schedule next message arrival
            j = nsim % 26;
            for (i = 0; i < 20; i++) msg2give.data[i] = 'a' + j;
            if (TRACE > 2) {
                printf("          MAINLOOP: data given to student: ");
                for (i = 0; i < 20; i++)
                    printf("%c", msg2give.data[i]);
                printf("\n");
            }
            nsim++;
            if (eventptr->eventity == A)
                A_output(msg2give);     // Call sender A's output handler
            else
                B_output(msg2give);     // Call sender B's output handler
        }
        else if (eventptr->evtype == FROM_LAYER3) {
            pkt2give = eventptr->packet;
            if (eventptr->eventity == A)
                A_input(pkt2give);      // Call receiver A's input handler
            else
                B_input(pkt2give);      // Call receiver B's input handler
        }
        else if (eventptr->evtype == TIMER_INTERRUPT) {
            if (eventptr->eventity == A)
                A_timerinterrupt();
            else
                B_timerinterrupt();
        }
        else {
            printf("INTERNAL PANIC: unknown event type\n");
        }
        free(eventptr);                 // Deallocate processed event
    }

    printf("Simulator terminated at time %f\n after sending %d msgs from layer5\n", time, nsim);
    printf("Total bytes received at layer5: %d\n", bytes_delivered_to_layer5);
    printf("Throughput: %.2f bytes/sec\n", bytes_delivered_to_layer5 / time);
    return 0;
}

/**
 * init - Initialize simulator environment and parameters
 * 
 * Sets up loss/corruption probabilities, random seed, generates first event,
 * validates random number generator for portability
 */
void init(void) {
    int i;
    float sum = 0.0, avg;

    printf("-----  Sliding Window Network Simulator -------- \n\n");
    printf("Drop Rate: %.2f, Corrupt Rate: %.2f (fixed)\n", LOSS_PROB, CORRUPT_PROB);

    lossprob = LOSS_PROB;
    corruptprob = CORRUPT_PROB;
    nsimmax = SIM_MESSAGE_MAX;
    lambda = TIME_AVERAGE;

    srand(9999);
    sum = 0.0;                /* Validate random number generator */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand();    /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75) {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(0);
    }

    ntolayer3 = 0;
    nlost = 0;
    ncorrupt = 0;
    time = 0.0;
    generate_next_arrival();
}

/**
 * jimsrand - Generate uniformly distributed random number in [0,1)
 * 
 * Returns: Random float value between 0 and 1
 */
float jimsrand() {
    float x;                   /* Individual implementations may require adjustments */
    x = ((float)rand()) / mmm;           /* Normalize to [0,1) range */
    return(x);
}

/**
 * generate_next_arrival - Schedule next message arrival from layer 5
 * 
 * Creates and inserts a new FROM_LAYER5 event with exponentially-distributed
 * inter-arrival time, randomly assigned to entity A or B
 */
void generate_next_arrival(void) {
    double x;
    struct event* evptr = (struct event*)malloc(sizeof(struct event));

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2;
    evptr->evtime = time + x;   // Schedule arrival time
    evptr->evtype = FROM_LAYER5;
    evptr->eventity = (BIDIRECTIONAL && (jimsrand() > 0.5)) ? B : A;
    insertevent(evptr);         // Insert into event list
}

/**
 * insertevent - Insert event into ordered event queue
 * @p: Event to insert
 * 
 * Maintains event list in chronological order (earliest event first)
 * using doubly-linked list for efficient insertion
 */
void insertevent(struct event* p) {
    struct event* q = evlist;
    struct event* qold = NULL;
    if (TRACE > 3) {
        printf("            INSERTEVENT: time is %lf\n", time);
        printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
    }

    if (q == NULL) {   /* List is empty */
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else {
        while (q != NULL && p->evtime > q->evtime) {
            qold = q;
            q = q->next;
        }
        if (q == NULL) {  /* Insert at end of list */
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        }
        else if (q == evlist) { /* Insert at front of list */
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        }
        else {  /* Insert in middle of list */
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

/********************** Student-callable ROUTINES ***********************/

/**
 * stoptimer - Cancel previously-started timer for entity
 * @AorB: Entity identifier (A=0 or B=1)
 * 
 * Searches event list for timer interrupt event belonging to specified entity
 * and removes it from the queue. Prints warning if timer not found.
 */
void stoptimer(int AorB) {
    struct event* q;

    if (TRACE>2)
        printf("          STOP TIMER: stopping timer at %f\n",time);
    
    /* Search for timer event in event queue */
    for (q = evlist; q != NULL; q = q->next) {
        if ((q->evtype == TIMER_INTERRUPT) && (q->eventity == AorB)) {

            /* Remove event from doubly-linked list */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL;          /* Only event in list */
            else if (q->next == NULL)
                q->prev->next = NULL;   /* Last event */
            else if (q == evlist) {
                q->next->prev = NULL;   /* First event */
                evlist = q->next;
            }
            else {
                q->next->prev = q->prev;    /* Middle of list */
                q->prev->next = q->next;
            }
            free(q);    /* Deallocate event memory */
            return;
        }
    }
    /* Timer was not found — may not have been started */
    printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

/**
 * starttimer - Start retransmission timer for entity
 * @AorB: Entity identifier (A=0 or B=1)
 * @increment: Timeout duration in milliseconds
 * 
 * Creates and schedules timer interrupt event. Prevents duplicate timers
 * for the same entity.
 */
void starttimer(int AorB, float increment) {
    struct event* q;
    struct event* evptr;

    if (TRACE>2)
        printf("          START TIMER: starting timer at %f\n",time);

    /* Check if timer already running for this entity */
    for (q = evlist; q != NULL; q = q->next) {
        if ((q->evtype == TIMER_INTERRUPT) && (q->eventity == AorB)) {
            printf("Warning: attempt to start a timer that is already started\n");
            return;
        }
    }

    /* Create and initialize timer event */
    evptr = (struct event*)malloc(sizeof(struct event));
    evptr->evtime = time + increment;   /* Expiration time */
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;
    insertevent(evptr);     /* Insert into event queue */
}

/**
 * tolayer3 - Send packet to network layer (layer 3)
 * @AorB: Sender entity identifier
 * @pkt: Packet to transmit
 * 
 * Simulates network transmission: applies loss probability, network delay,
 * and corruption probability. Schedules delivery event on receiver side.
 */
void tolayer3(int AorB, struct pkt pkt) {
    struct event* evptr;
    struct event* q;
    float lastime;
    int i;
    float x;

    ntolayer3++;    /* Increment packet count */

    /* Simulate packet loss */
    if (jimsrand() < lossprob) {
        nlost++;
        if (TRACE > 0)
            printf("          TOLAYER3: packet %d being lost\n", pkt.seqnum);
        return;
    }

    /* Create delivery event for receiver side */
    evptr = (struct event*)malloc(sizeof(struct event));
    evptr->evtype = FROM_LAYER3;
    evptr->eventity = (AorB + 1) % 2;  /* Deliver to opposite entity (A ↔ B) */

    evptr->packet = pkt;  /* Copy entire packet */

    if (TRACE > 2) {
        printf("          TOLAYER3: check: %d ", evptr->packet.checksum);
        for (i = 0; i < 20; i++)
            printf("%c", evptr->packet.payload[i]);
        printf("\n");
    }    

    /* Schedule delivery with network delay simulation */
    lastime = time;
    for (q = evlist; q != NULL; q = q->next) {
        if (q->evtype == FROM_LAYER3 && q->eventity == evptr->eventity) {
            lastime = q->evtime;    /* Stack delays for in-order delivery */
        }
    }
    evptr->evtime = lastime + 1 + 9 * jimsrand();   /* Add random delay */

    /* Simulate packet corruption */
    if (jimsrand() < corruptprob) {
        ncorrupt++;
        x = jimsrand();
        if (x < 0.75)
            evptr->packet.payload[0] = 'Z';  /* Corrupt payload */
        else if (x < 0.875)
            evptr->packet.seqnum = 999999;   /* Corrupt sequence number */
        else
            evptr->packet.acknum = 999999;   /* Corrupt acknowledgment number */
        if (TRACE > 0)
            printf("          TOLAYER3: packet %d being corrupted\n", pkt.seqnum);
    }

    if (TRACE > 2) {
        printf("          TOLAYER3: scheduling arrival on other side at time %f\n", evptr->evtime);
    }

    insertevent(evptr); /* Add to simulator event queue */
}

/**
 * tolayer5 - Deliver data to application layer (layer 5)
 * @AorB: Receiving entity identifier
 * @datasent: Received data payload (20 bytes)
 * 
 * Delivers correctly received data to application layer and updates
 * throughput statistics.
 */
void tolayer5(int AorB, char datasent[20]) {
    int i;
    bytes_delivered_to_layer5 += 20; /* Update throughput counter */

    if (TRACE > 2) {
        printf("          TOLAYER5: data received: ");
        for (i = 0; i < 20; i++)
            printf("%c", datasent[i]);
        printf("\n");
    }
}

/**
 * printevlist - Display contents of event queue (debug utility)
 * 
 * Prints all pending events in chronological order with timestamps and types
 */
void printevlist(){
    struct event *q;
    int i;
    printf("--------------\nEvent List Follows:\n");
    for(q = evlist; q!=NULL; q=q->next) {
      printf("Event time: %f, type: %d entity: %d\n",q->evtime,q->evtype,q->eventity);
      }
    printf("--------------\n");
  }