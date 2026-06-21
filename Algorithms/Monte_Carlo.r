# Monte Carlo Statistical Analysis in R

# Set random seed for reproducibility
set.seed(123)

# Parameters
n_simulations <- 10000
sample_size <- 100
true_mean <- 50
true_sd <- 10

# Vector to store sample means
sample_means <- numeric(n_simulations)

# Monte Carlo Simulation
for(i in 1:n_simulations) {
  sample <- rnorm(sample_size,
                  mean = true_mean,
                  sd = true_sd)
  
  sample_means[i] <- mean(sample)
}

# Statistical Analysis
estimated_mean <- mean(sample_means)
estimated_sd <- sd(sample_means)

# 95% Confidence Interval
ci <- quantile(sample_means,
               probs = c(0.025, 0.975))

# Results
cat("Monte Carlo Statistical Analysis\n")
cat("---------------------------------\n")
cat("Number of simulations:", n_simulations, "\n")
cat("Estimated Mean:", round(estimated_mean, 4), "\n")
cat("Standard Error:", round(estimated_sd, 4), "\n")
cat("95% Confidence Interval:\n")
print(ci)

# Histogram
hist(sample_means,
     breaks = 50,
     col = "lightblue",
     main = "Monte Carlo Distribution of Sample Means",
     xlab = "Sample Mean",
     ylab = "Frequency")

abline(v = estimated_mean,
       col = "red",
       lwd = 2)

abline(v = ci[1],
       col = "darkgreen",
       lwd = 2,
       lty = 2)

abline(v = ci[2],
       col = "darkgreen",
       lwd = 2,
       lty = 2)

legend("topright",
       legend = c("Estimated Mean", "95% CI"),
       col = c("red", "darkgreen"),
       lty = c(1, 2),
       lwd = 2)
