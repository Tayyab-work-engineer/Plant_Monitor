# Plant_Monitor

Most beginner embedded projects use simple sensor thresholds with basic if/else logic. 

This project combines lookup tables for plant-specific parameters with binary search algorithms for historical pattern matching and adaptive learning.

# Requirements
Plant Profile Management: Define struct-based lookup table accessible via direct array indexing.

Smart Sensor Classification: Implement binary search through sorted threshold arrays to efficiently categorize continuous moisture/light/temperature readings into discrete levels.

Historical Pattern Learning: Store timestamped sensor data in sorted arrays and use binary search to find similar past conditions for predictive decision making.


