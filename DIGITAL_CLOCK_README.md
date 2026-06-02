# 🕐 World Digital Clock

A modern, responsive digital clock application that displays the current time in 12 different time zones around the world. Built with React and styled with glassmorphism effects.

## ✨ Features

- **🌍 12 Global Time Zones**: New York, London, Paris, Tokyo, Sydney, Dubai, Singapore, Hong Kong, Mumbai, São Paulo, Los Angeles, Toronto
- **⏰ Real-time Updates**: Clock updates every second
- **📍 Timezone Offsets**: Shows UTC offset for each timezone
- **📅 Local Date Display**: Shows your local date in readable format
- **🎨 Modern UI**: Glassmorphism design with smooth animations
- **📱 Fully Responsive**: Optimized for desktop, tablet, and mobile
- **🌓 Dark/Light Theme Support**: Adapts to system preference
- **⚡ Performance Optimized**: Smooth animations and efficient rendering

## 🛠️ Tech Stack

- **React 18** - UI framework
- **Vite** - Build tool & dev server
- **CSS3** - Advanced styling (animations, gradients, blur effects)
- **JavaScript ES6+** - Modern JavaScript

## 📦 Installation

### Prerequisites
- Node.js 14+ 
- npm or yarn

### Setup

1. **Clone the repository**
```bash
git clone https://github.com/yesitswalid/CourseOrientation.git
cd CourseOrientation
```

2. **Install dependencies**
```bash
npm install
```

3. **Start development server**
```bash
npm run dev
```

The app will open at `http://localhost:3000`

4. **Build for production**
```bash
npm run build
```

## 🎯 Usage

Simply open the application and it will display:
1. **Your Local Time** - Large prominent display of current time
2. **12 World Time Zones** - Grid showing time in different cities

The clock updates in real-time, and each timezone card shows:
- City name
- Current time (HH:MM:SS)
- UTC offset
- Timezone code

## 🎨 Customization

### Add/Remove Timezones

Edit the `timeZones` array in `DigitalClock.jsx`:

```jsx
const timeZones = [
  { name: 'New York', zone: 'America/New_York' },
  { name: 'London', zone: 'Europe/London' },
  // Add more as needed
];
```

### Modify Styling

- Colors: Edit CSS variables in `DigitalClock.css`
- Animations: Customize animation keyframes
- Layout: Adjust grid template columns in media queries

### Change Update Frequency

In `DigitalClock.jsx`, modify the interval:

```jsx
const timer = setInterval(() => {
  setCurrentTime(new Date());
}, 1000); // 1000ms = 1 second
```

## 📱 Responsive Breakpoints

- **Desktop**: Full grid (1400px+)
- **Tablet**: 2-3 columns (768px - 1200px)
- **Mobile**: Single column (below 768px)
- **Small Phone**: Optimized (below 480px)

## 🌐 Supported Timezones

The app supports all [IANA timezone database](https://www.iana.org/time-zones) timezones. Current includes:

- **Americas**: New York, Los Angeles, Toronto, São Paulo
- **Europe**: London, Paris
- **Asia**: Tokyo, Hong Kong, Singapore, Dubai, Mumbai
- **Oceania**: Sydney

## 🚀 Performance

- **Efficient Re-rendering**: Uses React hooks efficiently
- **Memory Management**: Proper cleanup of intervals
- **CSS Animations**: GPU-accelerated for smooth performance
- **Optimized Bundle**: Minified production build ~50KB

## 🎨 Design Features

### Visual Effects
- Glassmorphism cards with backdrop blur
- Neon glow text effects
- Smooth hover animations
- Gradient backgrounds
- Staggered card animations

### Accessibility
- High contrast colors for readability
- Semantic HTML structure
- Keyboard accessible
- Respects system color scheme preference

## 📊 Component Structure

```
App.jsx
└── DigitalClock.jsx
    ├── Local Time Card
    └── Timezone Cards Grid (12 cards)
```

## 🔧 Development

### Project Structure
```
├── DigitalClock.jsx      # Main clock component
├── DigitalClock.css      # Styling
├── App.jsx              # App wrapper
├── index.jsx            # React entry point
├── index.html           # HTML template
├── package.json         # Dependencies
└── vite.config.js       # Vite configuration
```

### Key Functions

**formatTime()** - Formats time for specific timezone
```jsx
const formatTime = (date, timeZone) => {
  return new Intl.DateTimeFormat('en-US', {
    timeZone: timeZone,
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit',
    hour12: true,
  }).format(date);
};
```

**getTimeOffset()** - Gets UTC offset for timezone
```jsx
const getTimeOffset = (timeZone) => {
  // Returns string like "GMT+5:30"
};
```

## 🐛 Troubleshooting

### Clock not updating
- Check browser console for errors
- Ensure JavaScript is enabled
- Clear browser cache and reload

### Timezone showing "Invalid TZ"
- Verify timezone name is correct (IANA format)
- Check for typos in timezone string

### Styling issues
- Clear CSS cache
- Check browser compatibility (Chrome, Firefox, Safari, Edge)
- Ensure no browser extensions interfering with styles

## 🌍 Browser Support

| Browser | Version |
|---------|---------|
| Chrome | 90+ |
| Firefox | 88+ |
| Safari | 14+ |
| Edge | 90+ |
| Opera | 76+ |

## 📈 Future Enhancements

- [ ] Add timezone search functionality
- [ ] Allow custom timezone selection
- [ ] Display sunrise/sunset times
- [ ] Add alarm functionality
- [ ] Show weather for each timezone
- [ ] Save favorite timezones to localStorage
- [ ] Add analog clock view option
- [ ] Export time in different formats
- [ ] Dark mode toggle
- [ ] Multiple language support

## 📝 License

This project is licensed under the Apache 2.0 License - see LICENSE file for details.

## 👨‍💻 Author

Created by [@yesitswalid](https://github.com/yesitswalid)

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

### How to Contribute
1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📞 Support

For support, open an issue on [GitHub Issues](https://github.com/yesitswalid/CourseOrientation/issues)

## 🎯 Quick Links

- [Live Demo](https://world-digital-clock.vercel.app) (when deployed)
- [Documentation](./README.md)
- [GitHub Repository](https://github.com/yesitswalid/CourseOrientation)

---

**Made with ❤️ for developers who work across time zones**

🕐 **Keep time with the world!**
