#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


int main(int argc, char *argv[])
{

  /* Check if arguments are valid */
  if (argc != 3)
  {
    fprintf(stderr, "requires [numVoters][numRounds]\n");
    return -1;
  }
  if (atoi(argv[1]) < 3)
  {
    fprintf(stderr, "Number of voters needs to be greater than 3\n");
    return -1;
  }
  if (atoi(argv[2]) < 1)
  {
    fprintf(stderr, "Number of rounds needs to be greater than 1\n");
    return -1;
  }

  /* Convert the arguments from strings to ints */
  int numVotes = atoi(argv[1]);
  int numRounds = atoi(argv[2]);

  /* Create pipes for n voters */
  int pipeCP[2 * numVotes];
  for (int i = 0; i < numVotes; i++)
  {
    pipe(&pipeCP[2*i]);
  }

  int nbytes;
  pid_t p;

  /* Begin the voting rounds */
  for (int i = 0; i < numRounds; i++)
  {
    printf("Round %d:\n", i+1);
    int total, votesY = 0;
    for (int j = 0; j < numVotes; j++)
    {

      /* fork the process */
      if ((p = fork()) < 0)
      {
        fprintf(stderr, "%s\n", "Fork Failed");
      }

      /* Parent Process */
      else if (p > 0)
      {
        int result;
        /* Read from child process */
        size_t n = read(pipeCP[2*j], &result, sizeof(result));
        wait(NULL);

        /* adjust values based on child response */
        if (result == 1)
        {
          votesY++;
          total++;
        }
        else
        {
          total--;
        }
      }

      /* Child Process */
      else
      {
        /* seed random number generator with pid */
        srand(getpid());

        /* generate a random int either 0 or 1 */
        int vote = rand()%2;

        if (vote)
        {
          printf("PID %d %s\n", getpid(), "voted Yes");
        }
        else
        {
          printf("PID %d %s\n", getpid(), "voted No");
        }

        /* Send value to parent and exit */
        write (pipeCP[2*j + 1], &vote, sizeof(vote));
        exit(0);
      }
    }


    /* Output the results */
    printf("------------------\n");
    printf ("%d votes for Yes\n", votesY);
    printf ("%d votes for No\n", numVotes-votesY);
    if (total > 0)
    {
      printf("Majority voted Yes\n");
    }
    else if (total < 0)
    {
      printf("Majority voted No\n");
    }
    else
    {
      printf("Tie\n");
    }
    printf("\n");
  }


  exit(0);
}
