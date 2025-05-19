import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.widgets import SpanSelector, Button, Slider
from datetime import datetime

banlist = ['IDLE', 'Tmr Svc'] #ignoring threads

df = pd.read_csv('input.csv')
df.sort_values(by='time_ms', inplace=True)
df = df[~df['task'].isin(banlist)]

processes = df['task'].unique()

plt.rcParams['toolbar'] = 'None'

fig = plt.figure(figsize=(12, 8))
ax = plt.subplot2grid((10, 1), (0, 0), rowspan=7)
plt.subplots_adjust(bottom=0.05)

process_categories = {p: i for i, p in enumerate(processes)}
df['process_num'] = df['task'].map(process_categories)

total_min = df['time_ms'].min()
total_max = df['time_ms'].max()
current_xrange = total_max - total_min
slider_max = total_max
current_ylim = (min(process_categories.values()) - 1, max(process_categories.values()) + 1)


for process in processes:
    process_data = df[df['task'] == process]
    for i in range(len(process_data) - 1):
        current = process_data.iloc[i]
        next_event = process_data.iloc[i + 1]
        if current['event'] == 'in':
            start_time = current['time_ms']
            end_time = next_event['time_ms']
            ax.barh(process_categories[process], end_time - start_time, left=start_time, height=0.1, color='green', edgecolor='black')

ax.set_yticks(list(process_categories.values()))
ax.set_yticklabels(list(process_categories.keys()))
ax.set_title('Гонка процессов')
ax.set_xlabel('Время (мс)')
ax.set_ylabel('Название процесса')
ax.grid(True, axis='x', linestyle='--', alpha=0.5)
ax.set_xlim(total_min, total_max)
ax.set_ylim(current_ylim)

ax_slider = plt.axes([0.2, 0.1, 0.6, 0.03])
time_slider = Slider(
    ax=ax_slider,
    label='Время',
    valmin=total_min,
    valmax=total_max,
    valinit=total_min,
    valfmt='%d',
    valstep=int((total_max - total_min)/1000)
)
#ax_prev = plt.axes([0.3, 0.05, 0.1, 0.075])
#ax_next = plt.axes([0.41, 0.05, 0.1, 0.075])
ax_reset = plt.axes([0.15, 0.05, 0.1, 0.04])
ax_save = plt.axes([0.75, 0.05, 0.1, 0.04])

#btn_prev = Button(ax_prev, 'Назад')
#btn_next = Button(ax_next, 'Вперед')
btn_reset = Button(ax_reset, 'Сброс')
btn_save = Button(ax_save, 'Сохранить')


"""
def clamp_xlim(new_xlim):
    x_range = new_xlim[1] - new_xlim[0]
    
    if new_xlim[0] < total_min: new_xlim = (total_min, total_min + x_range)
    if new_xlim[1] > total_max: new_xlim = (total_max - x_range, total_max)
    
    return new_xlim

def on_prev(event):
    xlim = ax.get_xlim()
    xrange = xlim[1] - xlim[0]
    shift = min(xrange * 0.5, xlim[0] - total_min) 
    new_xlim = (xlim[0] - shift, xlim[1] - shift)
    ax.set_xlim(clamp_xlim(new_xlim))
    fig.canvas.draw()

def on_next(event):
    xlim = ax.get_xlim()
    xrange = xlim[1] - xlim[0]
    shift = min(xrange * 0.5, total_max - xlim[1]) 
    new_xlim = (xlim[0] + shift, xlim[1] + shift)
    ax.set_xlim(clamp_xlim(new_xlim))
    fig.canvas.draw()
"""
def update_slider(val):
    new_left = val
    new_right = min(new_left + current_xrange, total_max)
    if new_right > total_max:
        new_left = total_max - current_xrange
        new_right = total_max
    ax.set_xlim(new_left, new_right)
    fig.canvas.draw_idle()
    
def update_slider_limits():
    new_valmax = max(total_min, total_max - current_xrange)
    time_slider.valmax = new_valmax
    
    if time_slider.val > new_valmax:
        time_slider.set_val(new_valmax)
    
    time_slider.ax.set_xlim(time_slider.valmin, time_slider.valmax)

def on_reset(event):
    global current_xrange
    current_xrange = int(total_max - total_min)
    ax.set_xlim(total_min, total_max)
    ax.set_ylim(current_ylim)
    update_slider_limits()
    time_slider.set_val(total_min)
    fig.canvas.draw_idle()

def on_save(event):
    now = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    fig.savefig(f"process_race_{now}.png", dpi=300, bbox_inches='tight')
    print(f"График сохранен как process_race_{now}.png")

def on_scroll(event):
    global current_xrange
    
    if event.inaxes != ax:
        return
    
    prev_xrange = current_xrange
    
    scale_factor = 1.1 if event.button == 'up' else 1/1.1
    current_xrange *= scale_factor
    
    min_xrange = (total_max - total_min)/100
    max_xrange = (total_max - total_min)
    current_xrange = max(min_xrange, min(current_xrange, max_xrange))
    
    x_center = event.xdata if event.xdata else ax.get_xlim()[0] + prev_xrange/2
    new_left = x_center - current_xrange/2
    new_right = x_center + current_xrange/2
    
    if new_left < total_min:
        new_left = total_min
        new_right = total_min + current_xrange
    if new_right > total_max:
        new_right = total_max
        new_left = total_max - current_xrange
    
    ax.set_xlim(new_left, new_right)
    
    update_slider_limits()
    time_slider.set_val(new_left)
    fig.canvas.draw_idle()

#btn_prev.on_clicked(on_prev)
#btn_next.on_clicked(on_next)
time_slider.on_changed(update_slider)
btn_reset.on_clicked(on_reset)
btn_save.on_clicked(on_save)
fig.canvas.mpl_connect('scroll_event', on_scroll)

plt.gcf().canvas.manager.set_window_title('Визуализатор гонки процессов')
plt.show()