# rStrap UART/RTT Comms

## Installation

```bash
virtualenv --python python3 rstrap && source rstrap/bin/activate
pip install comms/
sudo sh -c "cat comms/tools/fusb > /usr/local/bin/fusb"
```

## Developer

Tmuxinator. First [install it](https://github.com/tmuxinator/tmuxinator), then

`tmuxinator new rstrap`

Copy paste the setup below over the default windows

```yaml
windows:
  - dev:
      layout: tiled
      panes:
        - cd ~/Documents/nRF5SDK1230d7731ad && python collect_data.py --device peripheral --channel uart
        - cd ~/Documents/nRF5SDK1230d7731ad && python collect_data.py --device central --channel uart
        - cd ~/Documents/nRF5SDK1230d7731ad && python collect_data.py --device peripheral --channel rtt
        - cd ~/Documents/nRF5SDK1230d7731ad && python collect_data.py --device central --channel rtt
  - logs: tail -f log/development.log
```

Tmux need-to-knows:

* Hold `shift` to copy in tmux as if it was a regular shell