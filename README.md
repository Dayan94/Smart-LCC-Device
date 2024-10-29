# Smart LCC Device

The repository contains the Arduino code written for the microcontroller programming for the device named 'Smart LCC Device.' Based on the Leaf Color Chart, the device is a urea measurement audio-visual IoT device for Bengali Farmers. It can measure urea for Aman and Boro paddy, maize, and wheat and send the data with the exact date, time, and location on an online site. The website then analyzes and visualizes the collected data using bar charts and heat maps. It has 25 buttons for user input, an LCD for the user interface, a speaker for audio output and instructions, and a GSM module for sending real-time data to an online site. The farmer needs to select a crop, input land size, and match the prescribed number of leaves' color taken from different places of the land with the six strips color chart attached to the device. Then, the device will display and tell whether the land requires urea provision and how much should be applied if necessary. 

---
## Tools and Technology Used

- Hardware Tools:
	1. Arduino UNO
	2. 16×2 LCD Display
	3. Micro SD Card Module
	4. GSM Shield SIM900
	5. 25 Push-Buttons
	6. Speaker
	7. Rechargeable Lithium-Ion Battery
	8. 10 K Ohm fixed resistors
	9. 10 K Ohm Potentiometers
	10. Voltage Regulator (Buck Converter and Boost Converter)

- Software Tools:
	1. Arduino Integrated Development Environment (IDE)
	2. Liquid Crystal Library (for LCD Display)
	3. TMRPCM library (for audio sound)
	4. Software Serial Library (for serial communication with GSM Shield SIM900)
	5. SD and SPI library (for functioning Micro SD Card)

---
## Achievements

1. I won the Championship in the Project Showcasing Contest at the Tech Fest 2017, Metropolitan University, excluding the IoT part of the device, only Aman and Boro paddy as crop input, and only bigha for the land size unit, adjustable by a single increment or decrement using the '+' and '-' buttons.

2. Later, I won the Championship in the Genius Hunt Senior Project Exhibition Contest at Cybernauts 2019, organized by North South University, Dhaka, Bangladesh, while adding for crops, maize and wheat along with Aman and Boro paddy; for land size, the number input buttons (0-9) instead of just incrementing or decrementing a single value using the '+' or '-' button, and katha, acre, and decimal units along with bigha (either bigha and katha units simultaneously while maintaining the order, otherwise exclusively, or acre and decimal units simultaneously while maintaining the order, otherwise exclusively); and for data storage and analysis, I implemented IoT features using a GSM module to send all the data to an online server with the exact date, time, and location. Moreover, I built a website using raw PHP and MySQL to store, analyze, and visualize the data sent by the device using bar charts and heatmaps to help the Bangladesh Agriculture Ministry monitor and manage countrywide urea provision effectively. 

3. After winning the Championship in the Genius Hunt Senior Project Exhibition Contest at Cybernauts 2019 and receiving acceptance of the research paper in IEEE Xplore after presenting it at the IEEE RAAICON 2019 Conference, I was awarded a "Certificate of Excellence" (Feb. 2020) from Metropolitan University for recognizing the outstanding contributions made to the institution's quality.

---
## Presentation Slide and Research Paper

I presented the research paper relevant to the device at the IEEE RAAICON 2019 Conference at FARS Hotel & Resorts, Akram Center, 212 Shahid Syed Nazrul Islam Sharani, Dhaka, Bangladesh. The presentation slide is available in the link here: [IEEE RAAICON 2019 Presentation Slide](./RAAICON-2019-Presentation-Slides_72.pdf).

To know details about the device, read the paper available in the given links below: 
- [IEEE Xplore Original Publication](https://ieeexplore.ieee.org/document/9087520)
- [Full Paper in ResearchGate](https://www.researchgate.net/publication/341400432_Smart_LCC_Device_LCC-Based_IoT_Device_for_measuring_urea_consumption_in_major_food_crops).