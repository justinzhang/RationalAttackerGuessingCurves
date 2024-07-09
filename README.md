# Rigorous Statistical Analysis of Empirical Password Datasets

This is am implementation of the statistical techniques described in [these papers](papers/). 

## Introduction

A central challenge in password security is to characterize an attacker’s guessing curve ($\lambda_{G}$) i.e., what is the probability that the attacker will crack a random user’s password within the first $G$ guesses. Guessing curves have important implication towards the development of robust security policies (determining the complexity of hashing algorithms, imposing password security policies, etc.) and the balance between security gains and usability costs. A key challenge is that the guessing curve depends on the attacker’s guessing strategy and the distribution of user passwords, both of which are unknown to us. In this work, we aim to analyze the performance of an **optimal** attacker who knows the underlying password distribution.

Let $\mathcal{P}$ denote an arbitrary password distribution over passwords ${pwd_{1}, pwd_{2}, \ldots}$ and $p_{i}$ denote the probability of sampling $pwd_{i}$. Without loss of generality, we can assume that the passwords are ordered by probability ($p_{1} \geq p_{2} \geq \cdots$). Intuitively, hen an **optimal** attacker who knows the distribution tries to crack a random password $s \leftarrow \mathcal{P}$ sampled from the distribution, the attacker will make guesses in the order $pwd_{1}, pwd_{2}, \ldots$ until success. The true guessing curve $\lambda_{G}$ can be calculated by summing the probability of the $G$ most probable passowrds in the distribution ($\lambda_{G} = \sum_{i \leq G} p_{i}$). Unfortunately, the password distribution $\mathcal{P}$ is unknown to us, so we need to apply statistical techniques to help estimate the true value.

In our application, we focus on generating tight upper and lower bounds for the guessing curve $\lambda_{G}$ of an optimal attacker. We stress that we make **no a priori assumptions** about the shape of the password distribution. Instead, our statistical framework only requires *iid* samples $S = {s_{1}, s_{2}, \ldots, s_{N}} \leftarrow \mathcal{P}^{N}$ from the **unknown** distribution. Formally, given samples $S$, guessing number $G$, and error rate $\delta$, our program can output $L$ and $U$ with the guarantee that $\Pr[L \leq \lambda_{G} \leq U] \geq 1 - \delta$.
 
### Specifying a Password Sample

The program can process password samples in 3 different file formats:

- **plain text**: Each line is one password.
- **password-frequency**: Each line is a password and an integer separated by a `\t` character, representing a **unique** password and the number of times it occurs in the sample (i.e. its frequency). To ensure correctness, make sure that no two lines contain the same passwords.
- **frequency-count**: Each line is two integers $x$ and $y$ separated by whitespace, indicating that there are $y$ unique passwords that all occur $x$ times in the sample.

> Note: A `"freqcount"` sample does not contain information about the actual passwords. Therefore, the `extended_LB` bound described in [the paper](papers/) is not available for such samples.

For example, consider a password sample containing 6 passwords: `{ "123456", "abcdef", "Password123", "abcdef" }`. In **plain text** format, it would be:

    123456
    abcdef
    Password123
    abcdef

In **password-frequency** format it would be:

    123456  1
    abcdef  2
    Password123  1

In **frequency-count** format it would be:

    2 1
    1 2

When interacting with the interface, the user can use strings `"plain"`, `"pwdfreq"`, or `"freqcount"` to indicate which format their password sample is represented with.

### Bounding Techniques

Our framework takes different approaches to generate tight bounds for guessing budgets $G$ of different magnitudes. The first 2 categories are described in [this paper](papers/LP_paper) and the last one is from [this paper](papers/PIN_paper).

- Partition the sample into two sets to simulate the process of attacking a random sample with partial knowledge of the distribution.
- Linear programming with Good-Turing estimates as constraints that hold with high probability.
- Apply properties of the binomial distribution to generate tight bounds.

Our program provides two interfaces. For those who want a quick summary without delving into too many technical details, the [simple interface](#simple-interface) automatically selects parameters that should work well in most settings; for those who wish to optimize parameters to obtain better results or perform customized experiments, the [advanced tools](#advanced-tools) allow full control over the parameters. Please also take a look at the [examples](examples/) for better guidance. For instructions on building and executing the program, see [this section](#building-the-program).

<!-- finish stuff above this -->

## Simple Interface

## Advanced Tools

### Working with Password Samples

The program utilizes the data structure `dist_t` to represent password samples from and store precomputed information about the samples. 

#### Creating a `dist_t` object

#### Partition a sample

#### Attacking a sample

### Calculating Bounds

### Plotting the Guessing Curves

## Building the Program

## Other Notes
- Please keep the password file associated with a `dist_t` object available and unchanged as long as the object is still in use. The program might read from the file to calculate bounds.
- For consistency and to ensure compilation success, please use `int64_t` for **every** integer variable in the program.

