This version adds:

- Integration of the LM35 temperature sensor to read real temperature values
- Simulated readings for moisture and light (until real sensors are added)
- Classification of real and simulated values using plant-specific thresholds
- UART-based output to display readings and their status (Too Low, Optimal, Too High)
- Button-triggered logic to start classification
- LED indicator lights up when button is pressed

This is a milestone towards full sensor integration and preparing for plant profile selection using binary search.

