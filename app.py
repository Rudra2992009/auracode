from flask import Flask, render_template, request, send_file
import yt_dlp
import os

app = Flask(__name__)

@app.route('/')
def index():
    # This serves the HTML file below
    return render_template('index.html')

@app.route('/download', methods=['POST'])
def download_video():
    url = request.form.get('url')
    # Use the specific URL you provided if the input is empty
    if not url:
        url = "https://music.youtube.com/watch?v=ANk2rYwEngk&si=mhv1UuJf_b9-eqdm"
    
    ydl_opts = {
        'format': 'bestaudio/best', # Downloads best audio quality
        'outtmpl': 'downloaded_song.%(ext)s',
        'postprocessors': [{
            'key': 'FFmpegExtractAudio',
            'preferredcodec': 'mp3',
            'preferredquality': '192',
        }],
    }

    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        ydl.download([url])
    
    return send_file('downloaded_song.mp3', as_attachment=True)

if __name__ == '__main__':
    app.run(debug=True)
