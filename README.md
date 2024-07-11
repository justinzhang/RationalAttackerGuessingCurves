# Rigorous Statistical Analysis of Empirical Password Datasets

This is am implementation of the statistical techniques described in this paper [these papers](papers/) and in a unpublished manuscript by the same authors. 

## Introduction

A central challenge in password security is to characterize an attacker’s guessing curve ($\lambda_{G}$) i.e., what is the probability that the attacker will crack a random user’s password within the first $G$ guesses. Guessing curves have important implication towards the development of robust security policies (determining the complexity of hashing algorithms, imposing password security policies, etc.) and the balance between security gains and usability costs. A key challenge is that the guessing curve depends on the attacker’s guessing strategy and the distribution of user passwords, both of which are unknown to us. This line of work aim sto follow Kerckhoffs' principle by analyzing the performance of an **optimal** attacker who knows the underlying password distribution.

Let $\mathcal{P}$ denote an arbitrary password distribution over passwords $\lbrace pwd_{1}, pwd_{2}, pwd_{3}, \ldots \rbrace$ and $p_{i}$ denote the probability of sampling $pwd_{i}$. Without loss of generality, we can assume that the passwords are ordered by probability ($p_{1} \geq p_{2} \geq \cdots$). Intuitively, hen an **optimal** attacker who knows the distribution tries to crack a random password $s \leftarrow \mathcal{P}$ sampled from the distribution, the attacker will make guesses in the order $pwd_{1}, pwd_{2}, \ldots$ until success. The true guessing curve $\lambda_{G}$ can be calculated by summing the probability of the $G$ most probable passowrds in the distribution ($\lambda_{G} = \sum_{i \leq G} p_{i}$). Unfortunately, the password distribution $\mathcal{P}$ is unknown to us, so we need to apply statistical techniques to help estimate the true value.

In our application, we focus on generating tight upper and lower bounds for the guessing curve $\lambda_{G}$ of an optimal attacker. We stress that we make **no a priori assumptions** about the shape of the password distribution. Instead, our statistical framework only requires *iid* samples $S = \lbrace s_{1}, s_{2}, \ldots, s_{N} \rbrace \leftarrow \mathcal{P}^{N}$ from the **unknown** distribution. Formally, given samples $S$, guessing number $G$, and error rate $\delta$, our program can output $L$ and $U$ with the guarantee that $\Pr[L \leq \lambda_{G} \leq U] \geq 1 - \delta$
 
### Specifying a Password Sample

The program can process password samples in 3 different file formats:

- **plain text**: Each line is one password.
- **password-frequency**: Each line is a password and an integer separated by a `\t` character, representing a **unique** password and the number of times it occurs in the sample (i.e. its frequency). To ensure correctness, make sure that no two lines contain the same passwords.
- **frequency-count**: Each line is two integers $x$ and $y$ separated by whitespace, indicating that there are $y$ unique passwords that all occur $x$ times in the sample.

> Note: A `"freqcount"` sample does not contain information about the actual passwords. Therefore, bounds that incorporate password cracking models cannot be applied to such datasets. Specifically, the `extended_LB` bound described in [the paper](papers/) is not available for such samples.

For example, consider a password sample containing 4 passwords: `{ "123456", "abcdef", "Password123", "abcdef" }`. In **plain text** format, it would be

    123456
    abcdef
    Password123
    abcdef

In **password-frequency** format it would be

    123456  1
    abcdef  2
    Password123  1

In **frequency-count** format it would be

    2 1
    1 2

When interacting with the interface, the user can use strings `"plain"`, `"pwdfreq"`, or `"freqcount"` to indicate which format their password sample is represented with.

### Bounding Techniques

Our framework takes different approaches to generate tight bounds for guessing budgets $G$ of different magnitudes. The first 2 categories are described in [this paper](papers/LP_paper) and the last one is from [this paper](papers/PIN_paper).

1. Empirical Distribution (Upper Bound): Use the empirical distribution to upper bound $\lambda_G$
2. Sample Partition (Lower Bound): Partition the sample $S$ into two parts $D_1$ and $D_2$. Use $D_1$ to build a cracking dictionary (ordered by frequency with which these passwords occur in $D_1$) and then measure the fraction of passwords in $D_2$ that would be cracked within $G$ guesses by an attacker that uses this dictionary. This lower bounds $\lambda_G$ as an attacker will perfect knowledge of the distribution can only outperform an attacker with partial knowledge of the distribution (the samples in $D_1$).
3. Linear Programming (Upper/Lower Bound): Find a distribution that maximizes (resp. minimizes) the quantity $\lambda_G$ subject to the constraint that the distribution is ``sufficiently consistent" with the underyling samples $S$ to obtain an upper (resp. lower) bound. Good-Turing frequency estimatation is used to generate linear constraints that hold with high probability.
4. Binomial PDF (Upper/Lower Bound): Apply properties of the binomial probability upper/lower bounds to generate upper/lower bounds. Empirical analysis indicates that the binomial PDF upper/lower bounds consistently outperforms the empirical distribution upper bound as well as the sample partition lower bound. Linear Programming provides the tightest upper/lower bounds when the guesing number is larger (e.g., when $G > N$ is larger than the number of samples). Binomial upper/lower bounds tend to outperform linear programming when the guessing number is lower. 

Our program provides two interfaces. For those who want a quick summary without delving into too many technical details, the [simple interface](#simple-interface) automatically selects parameters that should work well in most settings; for those who wish to optimize parameters to obtain better results or perform customized experiments, the [advanced tools](#advanced-tools) allow full control over the parameters. Please also take a look at the [examples](examples/) for better guidance. For instructions on building and executing the program, see [this section](#usage).

<!-- finish stuff above this -->

## Simple Interface

## Advanced Tools

### Working with Password Samples

The program utilizes the data structure `dist_t` to represent password samples and store precomputed information about the samples. 

#### Creating a `dist_t` object

- Use the `read_file(dist, filename, filetype)` function to associate a password sample stored in `filename` with the object `dist`. `filetype` should be either `"plain"`, `"pwdfreq"`, or `"freqcount"` depending on the format of the password sample file. The function returns `true` if the sample was successfully read and `false` if the file does not exist or couldn't be read.
- Use the `set_verbose(dist, true/false)` function to turn on/off error messages. Error messages are output to `stderr`, one can also redirect them to another file.
- Use the `write_freqcount(dist, filename)` function to write the password sample `dist` in the "freqcount" format to the file `filename`. This is useful since reading a "freqcount" file is much more efficient than reading a file with actual passwords. The function returns `true` is the data was successfully written to the file and `false` otherwise.

#### Partition a sample
 
- Use the `partition(dist, d)` function to randomly partition the sample set $S$ into $D_{1}$ and $D_{2}$ where $D_{2}$ is of size `d` and $D_{1}$ contains the remaining samples. The function returns `true` if the partition was succcessful and `false` otherwise.
- Use the `partition(dist, fraction)` function to randomly partition the sample set $S$ into $D_{1}$ and $D_{2}$ where $D_{2}$ is of size `d = N * fraction` (`N` is size of sample) and $D_{1}$ contains the remaining samples. The function returns `true` if the partition was succcessful and `false` otherwise.

It is recommended that the user keep the size of $D_{2}$ relatively small compared to the total number of samples (the [paper](lp_paper) uses $d = 25000$ with sample sets of size around $10^{8}$) to get the best bounds. Partitioning is required for bounds `samp_LB`, `extended_LB`, and `binom_LB`.

To retrieve the actual passwords in each partitioned set, use the function `partition(dist, d/fraction, D1_filename, D2_filename, filetype)` (note that this only applies to samples specified with "plain" or "pwdfreq" as "freqcount" samples don't contain actual passwords).


#### Attacking a sample

- attack()

### Calculating Bounds

- all overloads and wrappers

### Plotting the Guessing Curves


## Usage

### Building the Program

### Other Notes
- Please keep the password file associated with a `dist_t` object available and unchanged as long as the object is still in use. The program might read from the file to calculate bounds.
- For consistency and to ensure compilation success, please use `int64_t` for **every** integer variable in the program.

