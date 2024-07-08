# Rigorous Statistical Analysis of Empirical Password Datasets

This is am implementation of the statistical techniques described in [these papers](papers/)

Below is an introduction to using the interface. Please also take a look at the [examples](examples/) for better guidance.

## Background
A central challenge in password security is to characterize an attacker’s guessing curve ($\lambda_{G}$) i.e., what is the probability that the attacker will crack a random user’s password within the first $G$ guesses. Guessing curves have important implication towards the development of robust security policies (determining the complexity of hashing algorithms, imposing password security policies, etc.) and the balance between security gains and usability costs. A key challenge is that the guessing curve depends on the attacker’s guessing strategy and the distribution of user passwords, both of which are unknown to us. In this work, we aim to analyze the performance of an **optimal** attacker who knows the underlying password distribution.

Let $\mathcal{P}$ denote an arbitrary password distribution over passwords $\{pwd_{1}, pwd_{2}, \ldots\}$ and $p_{i}$ denote the probability of sampling $pwd_{i}$. Without loss of generality, we can assume that the passwords are ordered by probability i.e., $p_{1} \geq p_{2} \geq \cdots$. When an **optimal** attacker who knows the distribution tries to crack a random password $s \leftarrow \mathcal{P}$ sampled from the distribution, the attacker will make guesses in the order $pwd_{1}, pwd_{2}, \ldots$ until success. Intuitively, the true guessing curve $\lambda_{G}$ can be calculated by summing the probability of the $G$ most probable passowrds in the distribution i.e., $\lambda_{G} = \sum_{i=1}^{G} p_{i}$. Unfortunately, the password distribution $\mathcal{P}$ is unknown to us, so we need to utilize statistical techniques to help estimate the true value.

In our application, we focus on generating tight upper and lower bounds for the guessing curve $\lambda_{G}$ of an optimal attacker. We stress that we make no a priori assumptions about the shape of the password distribution. Instead, to apply our statistical framework, we only require *iid* samples $S = \{s_{1}, s_{2}, \ldots, s_{N}\} \leftarrow \mathcal{P}^{N}$ from the **unknown** distribution. Formally, given samples $S$, a guessing number $G$, and an error rate $\delta$, our program can output $L$ and $U$ with the guarantee that $\Pr[L \leq \lambda_{G} \leq U] \geq 1 - \delta$.
 
## Specifying Password Samples

The program utilizes the data structure `dist_t` to represent password samples from the underlying distribution and store precomputed information about. 

### Creating a `dist_t` object

### Partition a `dist_t` sample

### Attacking a `dist_t` sample

## Calculating Bounds

## Notes
- Please keep the file associated with a `dist_t` object available and unchanged as long as the object is still in use. The program might read from the file to calculate bounds.
- For consistency and to ensure compilation success, please use `int64_t` for **every** integer variable in the program.

